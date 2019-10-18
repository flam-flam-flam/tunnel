package main

import (
	utils "anytunnel/at-common"
	"bufio"
	"bytes"
	"crypto/tls"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"net"
	"net/http"
	"strings"
	"time"

	logger "github.com/snail007/mini-logger"
)

var (
	control utils.Author
	err     error
)
type Pxy struct {
	Cfg Cfg
}

// 设置type
type Cfg struct {
	Addr        string   // 监听地址
	Port        string   // 监听端口
	IsAnonymous bool     // 高匿名模式
	Debug       bool     // 调试模式
}
func init() {
	poster()
	initConfig()
	initLog()
}
func main() {
	defer func() {
		e := recover()
		if e != nil {
			log.Errorf("Exit ERR:%s", e)
		}
		logger.Flush()
	}()
	control, err = utils.NewAuthor(cfg.GetString("token"), cfg.GetString("host"), cfg.GetInt("port.control"), utils.CSTYPE_CLIENT)
	if err != nil {
		log.Debugf("create author fail : %s", err)
		return
	} //return author

	control.Channel.SetMsgErrorHandler(func(channel *utils.MessageChannel, msg interface{}, err error) {
		fmt.Printf("client err2222222:%s\n", err)
		log.Warnf("message pre-process error , ERRR:%s", err)
	})
	control.Channel.RegMsg(utils.MSG_TYPE_PONG, new(utils.MsgPong), func(channel *utils.MessageChannel, msg interface{}) {
		msgPong := msg.(*utils.MsgPong)
		log.Infof("pong revecived , id : %s", msgPong.ID)
		return
	})
	control.Channel.RegMsg(utils.MSG_CLIENT_OPEN_CONNECTION, new(utils.MsgClientOpenConnection), func(channel *utils.MessageChannel, msg interface{}) {
		msgClientOpenConnection := msg.(*utils.MsgClientOpenConnection)
		if msgClientOpenConnection.Protocol == utils.TUNNEL_PROTOCOL_TCP {
			openConnection(channel, msgClientOpenConnection)
		} else {
			openUDPConnection(channel, msgClientOpenConnection)
		}
	})
	control.Channel.DoServe(func(err error) {
		//log.Fatalf("offline , disconnected from cluster , %s", err)
		log.Fatalf("offline , disconnected from cluster .")
	})
	err = control.DoControlAuth()
	if err != nil {
		log.Fatalf("login fail : %s", err)
		return
	}
	//	log.Infof("login success %s - %s", control.Channel.LocalAddr(), control.Channel.RemoteAddr())
	// 参数
	faddr := flag.String("addr",cfg.GetString("httpaddr"),"监听地址，默认0.0.0.0")
	fprot := flag.String("port","50000","监听端口，默认8080")
	fanonymous :=  flag.Bool("anonymous",true,"高匿名，默认高匿名")
	fdebug :=  flag.Bool("debug",false,"调试模式显示更多信息，默认关闭")
	flag.Parse()

	cfg := &Cfg{}
	cfg.Addr = *faddr
	cfg.Port = *fprot
	cfg.IsAnonymous = *fanonymous
	cfg.Debug = *fdebug
	// fmt.Println(cfg)
	Run(cfg)
	/**
	/**
	 */
	log.Infof("login success")
	select {}
}
func Run(cfg *Cfg) {
	pxy := NewPxy()
	pxy.SetPxyCfg(cfg)
	log.Info("HttpPxoy is runing on %s:%s \n", cfg.Addr, cfg.Port)
	// http.Handle("/", pxy)
	bindAddr := cfg.Addr + ":" + cfg.Port
	log.Fatalln(http.ListenAndServe(bindAddr, pxy))
}
var clientClusterConnPool = utils.NewConcurrentMap()
// 实例化
func NewPxy() *Pxy {
	return &Pxy{
		Cfg: Cfg{
			Addr:        "",
			Port:        "8081",
			IsAnonymous: true,
			Debug:       false,
		},
	}
}

// 配置参数
func (p *Pxy) SetPxyCfg(cfg *Cfg) {
	if cfg.Addr != "" {
		p.Cfg.Addr = cfg.Addr
	}
	if cfg.Port != "" {
		p.Cfg.Port = cfg.Port
	}
	if cfg.IsAnonymous != p.Cfg.IsAnonymous {
		p.Cfg.IsAnonymous = cfg.IsAnonymous
	}
	if cfg.Debug != p.Cfg.Debug {
		p.Cfg.Debug = cfg.Debug
	}

}

// 运行代理服务
func (p *Pxy) ServeHTTP(rw http.ResponseWriter, req *http.Request) {
	// debug
	if p.Cfg.Debug {
		log.Info("Received request %s %s %s\n", req.Method, req.Host, req.RemoteAddr)
		// fmt.Println(req)
	}

	// http && https
	if req.Method != "CONNECT" {
		// 处理http
		p.HTTP(rw, req)
	} else {
		// 处理https
		// 直通模式不做任何中间处理
		p.HTTPS(rw, req)
	}

}

// http
func (p *Pxy) HTTP(rw http.ResponseWriter, req *http.Request) {

	transport := http.DefaultTransport

	// 新建一个请求outReq
	outReq := new(http.Request)
	// 复制客户端请求到outReq上
	*outReq = *req // 复制请求

	//  处理匿名代理
	if p.Cfg.IsAnonymous == false {
		if clientIP, _, err := net.SplitHostPort(req.RemoteAddr); err == nil {
			if prior, ok := outReq.Header["X-Forwarded-For"]; ok {
				clientIP = strings.Join(prior, ", ") + ", " + clientIP
			}
			outReq.Header.Set("X-Forwarded-For", clientIP)
		}
	}

	// outReq请求放到传送上
	res, err := transport.RoundTrip(outReq)
	if err != nil {
		rw.WriteHeader(http.StatusBadGateway)
		rw.Write([]byte(err.Error()))
		return
	}

	// 回写http头
	for key, value := range res.Header {
		for _, v := range value {
			rw.Header().Add(key, v)
		}
	}
	// 回写状态码
	rw.WriteHeader(res.StatusCode)
	// 回写body
	io.Copy(rw, res.Body)
	res.Body.Close()
}


// https
func (p *Pxy) HTTPS(rw http.ResponseWriter, req *http.Request) {

	// 拿出host
	host := req.URL.Host
	hij, ok := rw.(http.Hijacker)
	if !ok {
		log.Info("HTTP Server does not support hijacking")
	}

	client, _, err := hij.Hijack()
	if err != nil {
		return
	}

	// 连接远程
	server, err := net.Dial("tcp", host)
	if err != nil {
		return
	}
	client.Write([]byte("HTTP/1.0 200 Connection Established\r\n\r\n"))

	// 直通双向复制
	go io.Copy(server, client)
	go io.Copy(client, server)
}
func openUDPConnection(controlChannel *utils.MessageChannel, msg *utils.MsgClientOpenConnection) {
	tunnleID := msg.TunnelID
	connid := msg.ConnectinID
	connidStr := fmt.Sprintf("%d", (*msg).ConnectinID)
	var clusterConn *tls.Conn
	_, ok := clientClusterConnPool.Get(connidStr)
	if !ok {
		_clusterConn, err := connectCluster(*msg)
		if err != nil {
			return
		}
		clusterConn = &_clusterConn
		clientClusterConnPool.Set(connidStr, clusterConn)
		log.Debugf("connection %d - %d created success", tunnleID, connid)
		go func() {
			for {
				srcAddr, body, err := utils.ReadUDPPacket(clusterConn)
				if err == io.EOF || err == io.ErrUnexpectedEOF {
					log.Debugf("connection %d - %d released", tunnleID, connid)
					clientClusterConnPool.Remove(connidStr)
					break
				}
				func() {
					//log.Debugf("udp packet revecived:%s,%v", srcAddr, body)
					dstAddr := &net.UDPAddr{IP: net.ParseIP(msg.LocalHost), Port: msg.LocalPort}
					clientSrcAddr := &net.UDPAddr{IP: net.IPv4zero, Port: 0}
					conn, err := net.DialUDP("udp", clientSrcAddr, dstAddr)
					if err != nil {
						log.Warnf("connect to udp %s fail,ERR:%s", dstAddr.String(), err)
						return
					}
					conn.SetDeadline(time.Now().Add(time.Second * time.Duration(cfg.GetInt("udp.timeout"))))
					_, err = conn.Write(body)
					if err != nil {
						log.Warnf("send udp packet to %s fail,ERR:%s", dstAddr.String(), err)
						return
					}
					//log.Debugf("send udp packet to %s success", dstAddr.String())
					buf := make([]byte, 512)
					len, _, err := conn.ReadFromUDP(buf)
					if err != nil {
						log.Warnf("read udp response from %s fail ,ERR:%s", dstAddr.String(), err)
						return
					}
					respBody := buf[0:len]
					//log.Debugf("revecived udp packet from %s , %v", dstAddr.String(), respBody)
					_, err = clusterConn.Write(utils.UDPPacket(srcAddr, respBody))
					if err != nil {
						log.Warnf("send udp response to cluster fail ,ERR:%s", err)
						return
					}
					//log.Debugf("send udp response to cluster success ,from:%s", dstAddr.String())
				}()

			}
		}()
	}
	return
}
func openConnection(controlChannel *utils.MessageChannel, msg *utils.MsgClientOpenConnection) {
	tunnleID := msg.TunnelID
	connid := msg.ConnectinID
	clusterConn, err := connectCluster(*msg)
	if err != nil {
		return
	}
	localConn, err := utils.Connect(msg.LocalHost, msg.LocalPort, 5000)
	if err != nil {
		log.Debugf("connection %d - %d created fail ,err:%s", tunnleID, connid, err)
		clusterConn.Close()
		return
	}
	utils.IoBind(localConn, &clusterConn, func(err error) {
		localConn.Close()
		clusterConn.Close()
		log.Debugf("connection %d - %d released", tunnleID, connid)
	}, func(bytesCount int, isPositive bool) {}, 0)
	log.Debugf("connection %d - %d created success", tunnleID, connid)
	return
}
func connectCluster(cmd utils.MsgClientOpenConnection) (clusterConn tls.Conn, err error) {
	tunnleID := cmd.TunnelID
	connid := cmd.ConnectinID
	log.Debugf("new connection %d - %d", tunnleID, connid)
	clusterConn, err = utils.TlsConnect(cfg.GetString("host"), cfg.GetInt("port.conns"), 3000)
	if err != nil {
		log.Warnf("connect to cluster fail ,err:%s", err)
		return
	}
	writer := bufio.NewWriter(&clusterConn)
	pkg := new(bytes.Buffer)
	binary.Write(pkg, binary.LittleEndian, utils.CS_CLIENT)
	binary.Write(pkg, binary.LittleEndian, tunnleID)
	binary.Write(pkg, binary.LittleEndian, connid)
	writer.Write(pkg.Bytes())
	err = writer.Flush()
	if err != nil {
		log.Warnf("connect to cluster fail ,flush err:%s", err)
		return
	}
	return
}
