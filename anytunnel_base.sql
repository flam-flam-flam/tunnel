-- MySQL dump 10.13  Distrib 5.6.39, for Linux (x86_64)
--
-- Host: localhost    Database: anytunnel_base
-- ------------------------------------------------------
-- Server version	5.6.39-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `anytunnel_base`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `anytunnel_base` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `anytunnel_base`;

--
-- Table structure for table `at_area`
--

DROP TABLE IF EXISTS `at_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_area` (
  `area_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `name` varchar(50) NOT NULL COMMENT '区域名称',
  `cs_type` enum('server','client') NOT NULL COMMENT '类型,server或者client',
  `is_forbidden` tinyint(4) NOT NULL COMMENT '是否禁止,1:禁止0:允许',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`area_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='server和client登录区域黑白名单';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_area`
--

LOCK TABLES `at_area` WRITE;
/*!40000 ALTER TABLE `at_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `at_area` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_client`
--

DROP TABLE IF EXISTS `at_client`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_client` (
  `client_id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'client主键ID',
  `name` varchar(30) NOT NULL COMMENT 'client名称',
  `token` varchar(32) NOT NULL COMMENT 'client的token,最多32个字符',
  `user_id` varchar(32) NOT NULL COMMENT 'client所属用户ID,0代表是系统的client',
  `local_host` varchar(50) NOT NULL COMMENT '需要暴漏的本地网络host，client可以访问的ip域名都可以',
  `local_port` int(11) NOT NULL COMMENT '需要暴漏的本地网络host的端口',
  `create_time` int(11) NOT NULL COMMENT 'client创建时间',
  `update_time` int(11) NOT NULL COMMENT 'client更新时间',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否删除,1是,0否',
  PRIMARY KEY (`client_id`),
  UNIQUE KEY `token` (`token`),
  UNIQUE KEY `user_id_token` (`user_id`,`token`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 COMMENT='client表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_client`
--

LOCK TABLES `at_client` WRITE;
/*!40000 ALTER TABLE `at_client` DISABLE KEYS */;
INSERT INTO `at_client` VALUES (1,'client_gao','abcxxx','1','127.0.0.1',9999,0,0,0),(2,'test','hYZgLReEBvCXXYa2eH0o6egn9zSM6K2D','5','127.0.0.1',8889,1567408197,1567408197,0),(3,'c1','D3UMbk1ds2Xi3RP4BInLe0AqQqG7UzXn','5','127.0.0.1',8889,1567410623,1567410623,0),(4,'c1','FQ28x60SZygMQP6CgHyp0p5tMSefaOV5','2','127.0.0.1',8889,1567411004,1567411004,0),(5,'wqtest','TvSTfjGcCG2CuD3PLqYKbctqubIpBtjO','3','127.0.0.1',8889,1567477474,1567477474,0),(6,'52client','fMlRgwwMv04ucKeInOXsDMSCzGGTchoP','3','127.0.0.1',8889,1567583212,1567583212,0),(7,'boxclient','AX2Gf3Vav53Ewi0Npp8MuM2ZpCTv3E4x','3','127.0.0.1',8889,1567668107,1567668107,0),(9,'client_gaoc','mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw','4','127.0.0.1',8889,1568113905,1568113905,0);
/*!40000 ALTER TABLE `at_client` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_cluster`
--

DROP TABLE IF EXISTS `at_cluster`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_cluster` (
  `cluster_id` int(10) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `region_id` int(11) NOT NULL COMMENT '区域主键ID',
  `name` char(30) NOT NULL DEFAULT '' COMMENT '名称',
  `ip` char(15) NOT NULL DEFAULT '' COMMENT 'cluster的ip',
  `system_conn_number` int(10) NOT NULL DEFAULT '0' COMMENT '系统连接数',
  `tunnel_conn_number` int(10) NOT NULL DEFAULT '0' COMMENT '隧道连接数',
  `bandwidth` int(10) NOT NULL DEFAULT '0' COMMENT '带宽',
  `create_time` int(10) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(10) NOT NULL DEFAULT '0' COMMENT '修改时间',
  `is_disable` tinyint(1) NOT NULL COMMENT '是否禁用，0否，1是',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态 0 否 1 是',
  PRIMARY KEY (`cluster_id`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COMMENT='cluster机器表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_cluster`
--

LOCK TABLES `at_cluster` WRITE;
/*!40000 ALTER TABLE `at_cluster` DISABLE KEYS */;
INSERT INTO `at_cluster` VALUES (1,7,'cluster1','10.100.93.52',0,0,0,1567477599,0,0,0),(2,7,'localdev','10.100.106.116',0,0,0,1567477589,1567477637,0,0),(3,7,'公网测试','10.100.93.47',0,0,0,1567667950,0,0,0);
/*!40000 ALTER TABLE `at_cluster` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_conn`
--

DROP TABLE IF EXISTS `at_conn`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_conn` (
  `tunnel_id` int(11) NOT NULL COMMENT '用户的隧道主键ID',
  `user_id` int(11) NOT NULL COMMENT '用户主键ID',
  `cluster_id` int(11) NOT NULL COMMENT 'cluster主键ID',
  `server_id` int(11) NOT NULL COMMENT 'server主键ID',
  `count` int(11) NOT NULL COMMENT '用户的隧道的连接数',
  `upload` int(11) NOT NULL COMMENT '上传速度,字节/秒',
  `download` int(11) NOT NULL COMMENT '下载速度,字节/秒',
  `update_time` int(11) NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`tunnel_id`),
  KEY `count` (`count`),
  KEY `user_id_count` (`user_id`,`count`),
  KEY `server_id` (`server_id`),
  KEY `upload` (`upload`),
  KEY `download` (`download`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='连接数表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_conn`
--

LOCK TABLES `at_conn` WRITE;
/*!40000 ALTER TABLE `at_conn` DISABLE KEYS */;
INSERT INTO `at_conn` VALUES (5,3,2,5,1,0,0,1568951239);
/*!40000 ALTER TABLE `at_conn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_ip_list`
--

DROP TABLE IF EXISTS `at_ip_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_ip_list` (
  `ip_list_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `ip` char(15) NOT NULL COMMENT 'IP地址',
  `is_forbidden` tinyint(1) NOT NULL COMMENT '0禁止,1允许',
  `cs_type` enum('server','client') NOT NULL COMMENT 'cs类型,server或client',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`ip_list_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='cs登录IP黑白名单表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_ip_list`
--

LOCK TABLES `at_ip_list` WRITE;
/*!40000 ALTER TABLE `at_ip_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `at_ip_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_online`
--

DROP TABLE IF EXISTS `at_online`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_online` (
  `online_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '在线主键ID',
  `user_id` int(11) NOT NULL COMMENT '用户表主键ID',
  `cs_id` int(11) NOT NULL COMMENT 'server或者client的id',
  `cluster_id` int(11) NOT NULL COMMENT 'cluster的主键ID',
  `cs_ip` char(15) NOT NULL COMMENT 'server或者client的外网IP',
  `cs_type` enum('server','client') NOT NULL COMMENT '类型,server或client',
  `create_time` int(11) NOT NULL COMMENT '上线时间',
  PRIMARY KEY (`online_id`),
  UNIQUE KEY `token_type` (`cs_id`,`cs_type`),
  KEY `ip` (`cluster_id`)
) ENGINE=InnoDB AUTO_INCREMENT=440 DEFAULT CHARSET=utf8 COMMENT='server或client在线表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_online`
--

LOCK TABLES `at_online` WRITE;
/*!40000 ALTER TABLE `at_online` DISABLE KEYS */;
INSERT INTO `at_online` VALUES (68,3,6,2,'10.100.106.116','server',1567649892),(69,3,6,3,'10.100.93.52','client',1567680514),(98,4,8,1,'10.100.93.53','server',1568113145),(117,3,7,3,'10.100.93.47','server',1568267403),(325,3,5,2,'10.100.106.116','server',1568888926),(326,3,5,2,'10.100.106.116','client',1568888933),(433,3,7,2,'10.100.93.52','client',1571016703),(439,4,9,1,'10.100.93.53','server',1571118349);
/*!40000 ALTER TABLE `at_online` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_package`
--

DROP TABLE IF EXISTS `at_package`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_package` (
  `package_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '流量包主键ID',
  `user_id` int(11) NOT NULL COMMENT '用户主键ID',
  `bytes_total` bigint(20) NOT NULL COMMENT '流量包大小,单位字节',
  `bytes_left` bigint(20) NOT NULL COMMENT '剩余流量,单位字节',
  `start_time` int(11) NOT NULL COMMENT '生效时间',
  `end_time` int(11) NOT NULL COMMENT '过期时间',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL COMMENT '更新时间',
  `comment` char(10) NOT NULL COMMENT '流量包来源说明,最多10个字符',
  PRIMARY KEY (`package_id`),
  KEY `user_id_bytes_left` (`user_id`,`bytes_left`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户流量包表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_package`
--

LOCK TABLES `at_package` WRITE;
/*!40000 ALTER TABLE `at_package` DISABLE KEYS */;
/*!40000 ALTER TABLE `at_package` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_region`
--

DROP TABLE IF EXISTS `at_region`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_region` (
  `region_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '区域主键ID',
  `name` char(30) NOT NULL COMMENT '区域名称',
  `parent_id` int(11) NOT NULL COMMENT '上级区域的region_id,顶级区域为0',
  `create_time` int(11) NOT NULL COMMENT '区域创建时间',
  `update_time` int(11) NOT NULL COMMENT '区域修改时间',
  `is_delete` tinyint(1) NOT NULL COMMENT '是否删除,1是.0否',
  PRIMARY KEY (`region_id`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8 COMMENT='cluster区域表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_region`
--

LOCK TABLES `at_region` WRITE;
/*!40000 ALTER TABLE `at_region` DISABLE KEYS */;
INSERT INTO `at_region` VALUES (1,'香港',0,0,1503657697,0),(2,'香港免费1区',1,0,1503907304,0),(3,'新加坡',0,1503655210,1503657714,0),(4,'新加坡免费1区',3,1503656729,1503907288,0),(5,'大陆',0,1503658393,0,0),(7,'大陆VIP电信',5,1503658507,1503907337,0),(8,'香港VIP1区',1,1503674982,1503907795,0),(9,'大陆VIP联通',5,1503907177,1503907343,0),(10,'大陆VIP多线',5,1503907196,1503907398,0),(11,'大陆VIP移动',5,1503907217,1503907782,0),(12,'大陆免费1区',5,1503907253,1503907771,0),(13,'新加坡VIP1区',3,1503907816,1503907824,0),(14,'香港高速VIP1区',1,1503907869,1503909105,0),(15,'香港高速VIP2区',1,1503907873,0,0),(16,'日本',0,1503909138,0,0),(17,'日本免费1区',16,1503909142,0,0),(18,'日本VIP1区',16,1503909151,0,0),(19,'美国',0,1503909156,0,0),(20,'美国免费1区',19,1503909162,1503909170,0),(21,'美国VIP1区',19,1503909195,0,0);
/*!40000 ALTER TABLE `at_region` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_role`
--

DROP TABLE IF EXISTS `at_role`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_role` (
  `role_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '角色主键ID',
  `name` char(30) NOT NULL COMMENT '角色名称',
  `bandwidth` bigint(20) NOT NULL COMMENT '带宽限制,单位字节,0不限制',
  `server_area` enum('china','foreign','all') NOT NULL DEFAULT 'china' COMMENT 'server允许登录的区域,china:中国,foreign:国外,all:全部',
  `client_area` enum('china','foreign','all') NOT NULL DEFAULT 'china' COMMENT 'client允许登录的区域,china:中国,foreign:国外,all:全部',
  `tunnel_mode` char(5) NOT NULL COMMENT '允许的隧道模式0:普通1:高级2:特殊',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL COMMENT '修改时间',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否删除,1是,0否',
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COMMENT='角色表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_role`
--

LOCK TABLES `at_role` WRITE;
/*!40000 ALTER TABLE `at_role` DISABLE KEYS */;
INSERT INTO `at_role` VALUES (1,'默认组',100000,'china','china','0,2,1',0,1504602526,0),(2,'VIP1',500000,'china','china','0,1,2',0,1504509792,0);
/*!40000 ALTER TABLE `at_role` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_role_region`
--

DROP TABLE IF EXISTS `at_role_region`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_role_region` (
  `role_region_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '角色和区域关系主键ID',
  `role_id` int(11) NOT NULL COMMENT '角色主键ID',
  `region_id` int(11) NOT NULL COMMENT '区域主键ID',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  PRIMARY KEY (`role_region_id`)
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8 COMMENT='角色和区域关系表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_role_region`
--

LOCK TABLES `at_role_region` WRITE;
/*!40000 ALTER TABLE `at_role_region` DISABLE KEYS */;
INSERT INTO `at_role_region` VALUES (35,1,7,1504249580),(36,1,9,1504249580),(37,1,10,1504249580),(38,1,11,1504249580),(39,1,12,1504249580),(40,1,17,1504249580),(41,1,18,1504249580);
/*!40000 ALTER TABLE `at_role_region` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_server`
--

DROP TABLE IF EXISTS `at_server`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_server` (
  `server_id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'server主键ID',
  `name` varchar(30) NOT NULL COMMENT 'server名称',
  `token` varchar(32) NOT NULL COMMENT 'server的token,最多32个字符',
  `user_id` int(11) NOT NULL COMMENT 'server所属用户ID,0代表是系统的server',
  `create_time` int(11) NOT NULL COMMENT 'server创建时间',
  `update_time` int(11) NOT NULL COMMENT 'server更新时间',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否删除,1是,0否',
  PRIMARY KEY (`server_id`),
  UNIQUE KEY `token` (`token`),
  UNIQUE KEY `user_id_token` (`user_id`,`token`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 COMMENT='server表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_server`
--

LOCK TABLES `at_server` WRITE;
/*!40000 ALTER TABLE `at_server` DISABLE KEYS */;
INSERT INTO `at_server` VALUES (1,'server_gao','abcxxx',0,0,0,0),(2,'my','pTGQ95TIKoMfD53EtGbe4F8aV1iUMFBE',1,1567409568,1567409568,0),(3,'s1','aN2renVPLW4CfuBBz7mzFNVuVMC0jn7O',5,1567410632,1567410632,0),(4,'s1','e28GRAYXJvgeVJy9dVbEJBIFuVOK5hsM',2,1567411013,1567411013,0),(5,'wqserver','9kqdY3vVmQFINGVFAQOSiUWbOost7XW2',3,1567477483,1567477483,0),(6,'52server','CISzoJOxQgvCyNexMqclzBNycgZLma6f',3,1567583180,1567583180,0),(7,'47server','NagLGQCvaSseW8ImLXtsAdzQtOuXFkqG',3,1567668058,1567758097,0),(9,'server_gaoc','nUvqVCUn4bjmy5lVN9iqUlspPo5FpWnQ',4,1568113915,1568114277,0);
/*!40000 ALTER TABLE `at_server` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_tunnel`
--

DROP TABLE IF EXISTS `at_tunnel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_tunnel` (
  `tunnel_id` int(10) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `mode` tinyint(1) NOT NULL COMMENT '模式，0普通模式，1高级模式，2特殊模式',
  `name` char(255) NOT NULL DEFAULT '' COMMENT '隧道名称',
  `user_id` int(10) NOT NULL DEFAULT '0' COMMENT '隧道所属用户主键ID',
  `cluster_id` int(10) NOT NULL COMMENT 'cluster主键ID',
  `server_id` int(11) NOT NULL DEFAULT '0' COMMENT 'server主键ID',
  `client_id` int(11) NOT NULL COMMENT 'client主键ID',
  `protocol` tinyint(1) NOT NULL COMMENT '协议 1:TCP 2:UDP',
  `server_listen_port` int(10) NOT NULL DEFAULT '0' COMMENT 'server 监听端口',
  `server_listen_ip` char(100) NOT NULL DEFAULT '' COMMENT 'server bind ip',
  `client_local_port` int(10) NOT NULL DEFAULT '0' COMMENT 'client local port',
  `client_local_host` char(100) NOT NULL DEFAULT '' COMMENT 'client localhost',
  `status` tinyint(1) NOT NULL COMMENT '隧道状态,0异常,1:正常',
  `is_open` tinyint(1) NOT NULL COMMENT '是否已经打开,0否,1是,打开过的无论是否异常,必须手动一次关闭',
  `create_time` int(10) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(10) NOT NULL DEFAULT '0' COMMENT '修改时间',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态 0 否 1 是',
  PRIMARY KEY (`tunnel_id`),
  KEY `user_id` (`user_id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COMMENT='隧道表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_tunnel`
--

LOCK TABLES `at_tunnel` WRITE;
/*!40000 ALTER TABLE `at_tunnel` DISABLE KEYS */;
INSERT INTO `at_tunnel` VALUES (1,0,'tunnel_1',0,3,1,1,1,8888,'127.0.0.1',9999,'127.0.0.1',1,1,0,0,0),(4,1,'t1',2,1,4,4,1,8889,'192.168.0.10',5555,'192.168.11.12',1,1,1567411181,1567411181,0),(5,1,'localdev',3,2,5,5,1,22222,'10.100.106.116',22223,'10.100.106.116',1,1,1567477699,1568794286,0),(9,1,'box 测试',3,3,7,7,1,11111,'10.100.93.47',50000,'127.0.0.1',1,1,1567761813,1568168006,0),(11,1,'tunnel_gaoc',4,1,9,9,1,22222,'10.100.93.53',22223,'127.0.0.1',1,1,1568113996,1568164729,0);
/*!40000 ALTER TABLE `at_tunnel` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_user`
--

DROP TABLE IF EXISTS `at_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_user` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户主键ID',
  `nickname` char(20) NOT NULL COMMENT '昵称',
  `username` char(20) NOT NULL COMMENT '用户登录名',
  `password` char(32) NOT NULL COMMENT '密码',
  `email` char(50) NOT NULL COMMENT 'email地址',
  `is_active` tinyint(1) NOT NULL COMMENT 'email是否激活',
  `is_forbidden` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否禁用用户,1是.0否',
  `forbidden_reason` varchar(50) NOT NULL COMMENT '禁用的原因描述',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL COMMENT '修改时间',
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `username` (`username`),
  KEY `email` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COMMENT='用户表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_user`
--

LOCK TABLES `at_user` WRITE;
/*!40000 ALTER TABLE `at_user` DISABLE KEYS */;
INSERT INTO `at_user` VALUES (1,'','kingwang','e10adc3949ba59abbe56e057f20f883e','1@qq.com',0,0,'0000',1567409367,1567409367),(2,'','ylg','e10adc3949ba59abbe56e057f20f883e','11@22.com',0,0,'0000',1567410915,1567410915),(3,'','wqwin','e10adc3949ba59abbe56e057f20f883e','11@outlook.com',0,0,'0000',1567477457,1567477457),(4,'','gaochuang','416fa7933bb5d05831bda95a5dce5a9a','gaochuang@ukelink.com',0,0,'0000',1568111711,1568111711);
/*!40000 ALTER TABLE `at_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `at_user_role`
--

DROP TABLE IF EXISTS `at_user_role`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `at_user_role` (
  `role_user_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户和角色关系主键ID',
  `role_id` int(11) NOT NULL COMMENT '角色主键ID',
  `user_id` int(11) NOT NULL COMMENT '用户主键ID',
  `create_time` int(11) NOT NULL COMMENT '创建时间',
  `update_time` int(11) NOT NULL,
  PRIMARY KEY (`role_user_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COMMENT='用户和角色关系表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `at_user_role`
--

LOCK TABLES `at_user_role` WRITE;
/*!40000 ALTER TABLE `at_user_role` DISABLE KEYS */;
INSERT INTO `at_user_role` VALUES (1,1,1,1567409367,1567409367),(2,1,2,1567410915,1567410915),(3,1,3,1567477457,1567477457),(4,1,4,1568111711,1568111711);
/*!40000 ALTER TABLE `at_user_role` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-10-15 14:00:38
