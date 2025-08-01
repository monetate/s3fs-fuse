/*
 * s3fs - FUSE-based file system backed by Amazon S3
 *
 * Copyright(C) 2007 Takeshi Nakatani <ggtakec.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cstdio>
#include <cstring>
#include <string>

#include "s3objlist.h"

//-------------------------------------------------------------------
// Class S3ObjList
//-------------------------------------------------------------------
// New class S3ObjList is base on old s3_object struct.
// This class is for S3 compatible clients.
//
// If name is terminated by "/", it is forced dir type.
// If name is terminated by "_$folder$", it is forced dir type.
// If is_dir is true, the name ends with "/", or the name ends with "_$folder$",
// it will be determined to be a directory.
// If it is determined to be a directory, one of the directory types in objtype_t
// will be set to type. If it is not a directory(file, symbolic link), type will
// be set to objtype_t::UNKNOWN.
//
bool S3ObjList::insert(const char* name, const char* etag, bool is_dir)
{
    if(!name || '\0' == name[0]){
        return false;
    }

    s3obj_t::iterator iter;
    std::string newname;
    std::string orgname = name;
    objtype_t   type    = objtype_t::UNKNOWN;

    // Normalization
    std::string::size_type pos = orgname.find("_$folder$");
    if(std::string::npos != pos){
        newname = orgname.substr(0, pos);
        type    = objtype_t::DIR_FOLDER_SUFFIX;
    }else{
        newname = orgname;
    }
    if('/' == *newname.rbegin()){
        if(!IS_DIR_OBJ(type)){
            type = objtype_t::DIR_NORMAL;
        }
    }else{
        if(is_dir || IS_DIR_OBJ(type)){
            newname += "/";
            if(!IS_DIR_OBJ(type)){
                type = objtype_t::DIR_NOT_TERMINATE_SLASH;
            }
        }
    }

    // Check derived name object.
    if(is_dir || IS_DIR_OBJ(type)){
        std::string chkname = newname.substr(0, newname.length() - 1);
        if(objects.cend() != (iter = objects.find(chkname))){
            // found "dir" object --> remove it.
            objects.erase(iter);
        }
    }else{
        std::string chkname = newname + "/";
        if(objects.cend() != (iter = objects.find(chkname))){
            // found "dir/" object --> not add new object.
            // and add normalization
            return insert_normalized(orgname.c_str(), chkname.c_str(), type);
        }
    }

    // Add object
    if(objects.cend() != (iter = objects.find(newname))){
        // Found same object --> update information.
        iter->second.normalname.clear();
        iter->second.orgname = orgname;
        iter->second.type    = type;
        if(etag){
            iter->second.etag = etag;  // over write
        }
    }else{
        // add new object
        s3obj_entry newobject;
        newobject.orgname = orgname;
        newobject.type    = type;
        if(etag){
            newobject.etag = etag;
        }
        objects[newname] = newobject;
    }

    // add normalization
    return insert_normalized(orgname.c_str(), newname.c_str(), type);
}

bool S3ObjList::insert_normalized(const char* name, const char* normalized, objtype_t type)
{
    if(!name || '\0' == name[0] || !normalized || '\0' == normalized[0]){
        return false;
    }
    if(0 == strcmp(name, normalized)){
        return true;
    }

    s3obj_t::iterator iter;
    if(objects.cend() != (iter = objects.find(name))){
        // found name --> over write
        iter->second.orgname.clear();
        iter->second.etag.clear();
        iter->second.normalname = normalized;
        iter->second.type       = type;
    }else{
        // not found --> add new object
        s3obj_entry newobject;
        newobject.normalname = normalized;
        newobject.type       = type;
        objects[name]        = newobject;
    }
    return true;
}

const s3obj_entry* S3ObjList::GetS3Obj(const char* name) const
{
    s3obj_t::const_iterator iter;

    if(!name || '\0' == name[0]){
        return nullptr;
    }
    if(objects.cend() == (iter = objects.find(name))){
        return nullptr;
    }
    return &(iter->second);
}

std::string S3ObjList::GetOrgName(const char* name) const
{
    const s3obj_entry* ps3obj;

    if(!name || '\0' == name[0]){
        return "";
    }
    if(nullptr == (ps3obj = GetS3Obj(name))){
        return "";
    }
    return ps3obj->orgname;
}

std::string S3ObjList::GetNormalizedName(const char* name) const
{
    const s3obj_entry* ps3obj;

    if(!name || '\0' == name[0]){
        return "";
    }
    if(nullptr == (ps3obj = GetS3Obj(name))){
        return "";
    }
    if(ps3obj->normalname.empty()){
        return name;
    }
    return ps3obj->normalname;
}

std::string S3ObjList::GetETag(const char* name) const
{
    const s3obj_entry* ps3obj;

    if(!name || '\0' == name[0]){
        return "";
    }
    if(nullptr == (ps3obj = GetS3Obj(name))){
        return "";
    }
    return ps3obj->etag;
}

bool S3ObjList::IsDir(const char* name) const
{
    const s3obj_entry* ps3obj;

    if(nullptr == (ps3obj = GetS3Obj(name))){
        return false;
    }
    return IS_DIR_OBJ(ps3obj->type);
}

bool S3ObjList::GetLastName(std::string& lastname) const
{
    bool result = false;
    lastname = "";
    for(auto iter = objects.cbegin(); iter != objects.cend(); ++iter){
        if(!iter->second.orgname.empty()){
            if(lastname.compare(iter->second.orgname) < 0){
                lastname = iter->second.orgname;
                result = true;
            }
        }else{
            if(lastname.compare(iter->second.normalname) < 0){
                lastname = iter->second.normalname;
                result = true;
            }
        }
    }
    return result;
}

bool S3ObjList::RawGetNames(s3obj_list_t* plist, s3obj_type_map_t* pobjmap, bool OnlyNormalized, bool CutSlash) const
{
    if(!plist && !pobjmap){
        return false;
    }
    for(auto iter = objects.cbegin(); objects.cend() != iter; ++iter){
        if(OnlyNormalized && !iter->second.normalname.empty()){
            continue;
        }
        std::string name = iter->first;
        if(CutSlash && 1 < name.length() && '/' == *name.rbegin()){
            // only "/" std::string is skipped this.
            name.erase(name.length() - 1);
        }
        if(plist){
            plist->push_back(name);
        }
        if(pobjmap){
            (*pobjmap)[name] = iter->second.type;
        }
    }
    return true;
}

bool S3ObjList::GetNameList(s3obj_list_t& list, bool OnlyNormalized, bool CutSlash) const
{
    return RawGetNames(&list, nullptr, OnlyNormalized, CutSlash);
}

bool S3ObjList::GetNameMap(s3obj_type_map_t& objmap, bool OnlyNormalized, bool CutSlash) const
{
    return RawGetNames(nullptr, &objmap, OnlyNormalized, CutSlash);
}

typedef std::map<std::string, bool> s3obj_h_t;

bool S3ObjList::MakeHierarchizedList(s3obj_list_t& list, bool haveSlash)
{
    s3obj_h_t h_map;

    for(auto liter = list.cbegin(); list.cend() != liter; ++liter){
        std::string strtmp = (*liter);
        if(1 < strtmp.length() && '/' == *strtmp.rbegin()){
            strtmp.erase(strtmp.length() - 1);
        }
        h_map[strtmp] = true;

        // check hierarchized directory
        for(std::string::size_type pos = strtmp.find_last_of('/'); std::string::npos != pos; pos = strtmp.find_last_of('/')){
            strtmp.erase(pos);
            if(strtmp.empty() || "/" == strtmp){
                break;
            }
            if(h_map.cend() == h_map.find(strtmp)){
                // not found
                h_map[strtmp] = false;
            }
        }
    }

    // check map and add lost hierarchized directory.
    for(auto hiter = h_map.cbegin(); hiter != h_map.cend(); ++hiter){
        if(false == (*hiter).second){
            // add hierarchized directory.
            std::string strtmp = (*hiter).first;
            if(haveSlash){
                strtmp += "/";
            }
            list.push_back(strtmp);
        }
    }
    return true;
}

/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: expandtab sw=4 ts=4 fdm=marker
* vim<600: expandtab sw=4 ts=4
*/
