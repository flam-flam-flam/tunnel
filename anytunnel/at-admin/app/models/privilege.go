package models

import "github.com/snail007/go-activerecord/mysql"

type Privilege struct {
}

func (p *Privilege) GetTypedPrivileges(userId string, isDisplay string) (navigators, menus, controllers []map[string]string, err error) {
	db := G.DB()
	var rs *mysql.ResultSet
	if userId != "1" {
		roldIds := []string{}
		rs, err = db.Query(db.AR().Select("role_id").From("user_role").Where(map[string]interface{}{
			"user_id":   userId,
			"is_delete": 0,
		}))
		if err != nil {
			return
		}
		roldIds = rs.Values("role_id")

		privilegeIds := []string{}
		rs, err = db.Query(db.AR().Select("privilege_id").From("role_privilege").Where(map[string]interface{}{
			"role_id":   roldIds,
			"is_delete": 0,
		}))
		if err != nil {
			return
		}
		privilegeIds = rs.Values("privilege_id")

		rs, err = db.Query(db.AR().From("privilege").Where(map[string]interface{}{
			"privilege_id": privilegeIds,
		}).OrderBy("parent_id", "ASC").
			OrderBy("sequence", "ASC"))

		if err != nil {
			return
		}
	} else {
		rs, err = db.Query(db.AR().From("privilege").OrderBy("parent_id", "ASC").OrderBy("sequence", "ASC"))
		if err != nil {
			return
		}
	}
	navigators = []map[string]string{}
	menus = []map[string]string{}
	controllers = []map[string]string{}
	allPrivileges := rs.MapRows("privilege_id")
	for _, row := range rs.Rows() {
		if isDisplay != "-1" {
			if row["is_display"] != isDisplay {
				continue
			}
		}
		switch row["type"] {
		case "navigator":
			navigators = append(navigators, row)
		case "menu":
			row["parent"] = allPrivileges[row["parent_id"]]["name"]
			menus = append(menus, row)
		case "controller":
			row["parent"] = allPrivileges[row["parent_id"]]["name"]
			controllers = append(controllers, row)
		}
	}
	return
}
func (p *Privilege) GetPrivilegeByPrivilegeId(privilegeId string) (privilege map[string]string, err error) {
	db := G.DB()
	var rs *mysql.ResultSet
	rs, err = db.Query(db.AR().From("privilege").Where(map[string]interface{}{
		"privilege_id": privilegeId,
	}))
	if err != nil {
		return
	}
	privilege = rs.Row()
	return
}
func (p *Privilege) HasSub(privilegeId string) (has bool, err error) {
	db := G.DB()
	var rs *mysql.ResultSet
	rs, err = db.Query(db.AR().From("privilege").Where(map[string]interface{}{
		"parent_id": privilegeId,
	}).Limit(0, 1))
	if err != nil {
		return
	}
	if rs.Len() > 0 {
		has = true
	}
	return
}
func (p *Privilege) Delete(privilegeId string) (err error) {
	db := G.DB()
	_, err = db.Exec(db.AR().Delete("privilege", map[string]interface{}{
		"privilege_id": privilegeId,
	}))
	if err != nil {
		return
	}
	return
}
func (p *Privilege) Insert(privilege map[string]interface{}) (id int64, err error) {
	db := G.DB()
	var rs *mysql.ResultSet
	rs, err = db.Exec(db.AR().Insert("privilege", privilege))
	if err != nil {
		return
	}
	id = rs.LastInsertId
	return
}

func (p *Privilege) Update(privilegeId string, privilege map[string]interface{}) (id int64, err error) {
	db := G.DB()
	var rs *mysql.ResultSet
	rs, err = db.Exec(db.AR().Update("privilege", privilege, map[string]interface{}{
		"privilege_id": privilegeId,
	}))
	if err != nil {
		return
	}
	id = rs.LastInsertId
	return
}
