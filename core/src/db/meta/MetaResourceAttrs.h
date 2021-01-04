// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License.

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "db/meta/MetaFieldHelper.h"
#include "db/meta/MetaNames.h"
#include "db/meta/Utils.h"
#include "db/snapshot/ResourceContext.h"
#include "db/snapshot/Resources.h"
#include "utils/Json.h"
#include "utils/StringHelpFunctions.h"

namespace milvus::engine::meta {

// using namespace snapshot;
using snapshot::MappingT;
using snapshot::ResourceContext;

/////////////////////////// Macros ///////////////////////////////
#define NULLPTR_CHECK(ptr)                                                  \
    if (ptr == nullptr) {                                                   \
        return Status(SERVER_UNSUPPORTED_ERROR, "Convert pointer failed."); \
    }

//////////////////////////////////////////////////////////////////
Status
ResourceAttrMapOf(const std::string& table, std::vector<std::string>& attrs);

//////////////////////////////////////////////////////////////////
template <typename ResourceT>
inline Status
AttrValue2Str(typename ResourceContext<ResourceT>::ResPtr src, const std::string& attr, std::string& value) {
    int64_t int_value;
    uint64_t uint_value;
    snapshot::State state_value;
    MappingT mapping_value;
    std::string str_value;
    json json_value;

    if (attr == F_ID) {
        auto id_field = std::dynamic_pointer_cast<snapshot::IdField>(src);
        int_value = id_field->GetID();
        int2str(int_value, value);
    } else if (F_COLLECTON_ID == attr) {
        auto collection_id_field = std::dynamic_pointer_cast<snapshot::CollectionIdField>(src);
        int_value = collection_id_field->GetCollectionId();
        int2str(int_value, value);
    } else if (F_CREATED_ON == attr) {
        auto created_field = std::dynamic_pointer_cast<snapshot::CreatedOnField>(src);
        int_value = created_field->GetCreatedTime();
        int2str(int_value, value);
    } else if (F_UPDATED_ON == attr) {
        auto updated_field = std::dynamic_pointer_cast<snapshot::UpdatedOnField>(src);
        int_value = updated_field->GetUpdatedTime();
        int2str(int_value, value);
    } else if (F_SCHEMA_ID == attr) {
        auto schema_id_field = std::dynamic_pointer_cast<snapshot::SchemaIdField>(src);
        int_value = schema_id_field->GetSchemaId();
        int2str(int_value, value);
    } else if (F_NUM == attr) {
        auto num_field = std::dynamic_pointer_cast<snapshot::NumField>(src);
        int_value = num_field->GetNum();
        int2str(int_value, value);
    } else if (F_FTYPE == attr) {
        auto ftype_field = std::dynamic_pointer_cast<snapshot::FtypeField>(src);
        int_value = static_cast<int>(ftype_field->GetFtype());
        int2str(int_value, value);
    } else if (F_FETYPE == attr) {
        auto fetype_field = std::dynamic_pointer_cast<snapshot::FEtypeField>(src);
        int_value = static_cast<int>(fetype_field->GetFEtype());
        int2str(int_value, value);
    } else if (F_FIELD_ID == attr) {
        auto field_id_field = std::dynamic_pointer_cast<snapshot::FieldIdField>(src);
        int_value = field_id_field->GetFieldId();
        int2str(int_value, value);
    } else if (F_FIELD_ELEMENT_ID == attr) {
        auto element_id_field = std::dynamic_pointer_cast<snapshot::FieldElementIdField>(src);
        int_value = element_id_field->GetFieldElementId();
        int2str(int_value, value);
    } else if (F_PARTITION_ID == attr) {
        auto partition_id_field = std::dynamic_pointer_cast<snapshot::PartitionIdField>(src);
        int_value = partition_id_field->GetPartitionId();
        int2str(int_value, value);
    } else if (F_SEGMENT_ID == attr) {
        auto segment_id_field = std::dynamic_pointer_cast<snapshot::SegmentIdField>(src);
        int_value = segment_id_field->GetSegmentId();
        int2str(int_value, value);
    } /* Uint field */ else if (F_LSN == attr) {
        auto lsn_field = std::dynamic_pointer_cast<snapshot::LsnField>(src);
        uint_value = lsn_field->GetLsn();
        uint2str(uint_value, value);
    } else if (F_SIZE == attr) {
        auto size_field = std::dynamic_pointer_cast<snapshot::SizeField>(src);
        uint_value = size_field->GetSize();
        uint2str(uint_value, value);
    } else if (F_ROW_COUNT == attr) {
        auto row_count_field = std::dynamic_pointer_cast<snapshot::RowCountField>(src);
        uint_value = row_count_field->GetRowCount();
        uint2str(uint_value, value);
    } else if (F_STATE == attr) {
        auto state_field = std::dynamic_pointer_cast<snapshot::StateField>(src);
        state_value = state_field->GetState();
        state2str(state_value, value);
    } else if (F_MAPPINGS == attr) {
        if (auto flush_mappings_field = std::dynamic_pointer_cast<snapshot::FlushableMappingsField>(src)) {
            mapping_value = flush_mappings_field->GetFlushIds();
            mappings2str(mapping_value, value);
        } else if (auto mappings_field = std::dynamic_pointer_cast<snapshot::MappingsField>(src)) {
            mapping_value = mappings_field->GetMappings();
            mappings2str(mapping_value, value);
        }
    } else if (F_NAME == attr) {
        auto name_field = std::dynamic_pointer_cast<snapshot::NameField>(src);
        str_value = name_field->GetName();
        str2str(str_value, value);
    } else if (F_TYPE_NAME == attr) {
        auto type_name_field = std::dynamic_pointer_cast<snapshot::TypeNameField>(src);
        str_value = type_name_field->GetTypeName();
        str2str(str_value, value);
    } else if (F_PARAMS == attr) {
        auto params_field = std::dynamic_pointer_cast<snapshot::ParamsField>(src);
        json_value = params_field->GetParams();
        json2str(json_value, value);
    } else {
        return Status(SERVER_UNSUPPORTED_ERROR, "Unknown field attr: " + attr);
    }

    return Status::OK();
}

template <typename ResourceT>
inline Status
ResourceContextAddAttrMap(snapshot::ResourceContextPtr<ResourceT> src,
                          std::unordered_map<std::string, std::string>& attr_map) {
    std::vector<std::string> attrs;
    auto status = ResourceAttrMapOf(ResourceT::Name, attrs);
    if (!status.ok()) {
        return status;
    }

    for (auto& attr : attrs) {
        if (attr == F_ID) {
            continue;
        }

        std::string value;
        AttrValue2Str<ResourceT>(src->Resource(), attr, value);
        attr_map[attr] = value;
    }

    return Status::OK();
}

template <typename ResourceT>
inline Status
ResourceContextUpdateAttrMap(snapshot::ResourceContextPtr<ResourceT> res,
                             std::unordered_map<std::string, std::string>& attr_map) {
    std::string value;
    for (auto& attr : res->Attrs()) {
        AttrValue2Str<ResourceT>(res->Resource(), attr, value);
        attr_map[attr] = value;
    }

    return Status::OK();
}

template <typename ResourceT>
inline Status
AttrMap2Resource(std::unordered_map<std::string, std::string>& attr_map, typename ResourceT::Ptr& resource) {
    if (resource == nullptr) {
        return Status(DB_ERROR, "Resource is nullptr");
    }

    std::unordered_map<std::string, std::string>::iterator iter;
    if (auto fmf_p = std::dynamic_pointer_cast<snapshot::FlushableMappingsField>(resource)) {
        if ((iter = attr_map.find(F_MAPPINGS)) != attr_map.end()) {
            fmf_p->GetFlushIds() = Str2FieldValue<snapshot::MappingT>(iter->second);
        }
    } else if (auto mf_p = std::dynamic_pointer_cast<snapshot::MappingsField>(resource)) {
        if ((iter = attr_map.find(F_MAPPINGS)) != attr_map.end()) {
            mf_p->GetMappings() = Str2FieldValue<snapshot::MappingT>(iter->second);
        }
    }

    if (auto sf_p = std::dynamic_pointer_cast<snapshot::StateField>(resource)) {
        if ((iter = attr_map.find(F_STATE)) != attr_map.end()) {
            sf_p->ResetStatus();
            switch (Str2FieldValue<snapshot::State>(iter->second)) {
                case snapshot::PENDING: {
                    break;
                }
                case snapshot::ACTIVE: {
                    sf_p->Activate();
                    break;
                }
                case snapshot::DEACTIVE: {
                    sf_p->Deactivate();
                    break;
                }
                default: { return Status(SERVER_UNSUPPORTED_ERROR, "Invalid state value"); }
            }
        }
    }

    if (auto lsn_f = std::dynamic_pointer_cast<snapshot::LsnField>(resource)) {
        if ((iter = attr_map.find(F_LSN)) != attr_map.end()) {
            lsn_f->SetLsn(Str2FieldValue<snapshot::LSN_TYPE>(iter->second));
        }
    }

    if (auto created_on_f = std::dynamic_pointer_cast<snapshot::CreatedOnField>(resource)) {
        if ((iter = attr_map.find(F_CREATED_ON)) != attr_map.end()) {
            created_on_f->SetCreatedTime(Str2FieldValue<snapshot::TS_TYPE>(iter->second));
        }
    }

    if (auto update_on_p = std::dynamic_pointer_cast<snapshot::UpdatedOnField>(resource)) {
        if ((iter = attr_map.find(F_UPDATED_ON)) != attr_map.end()) {
            update_on_p->SetUpdatedTime(Str2FieldValue<snapshot::TS_TYPE>(iter->second));
        }
    }

    if (auto id_p = std::dynamic_pointer_cast<snapshot::IdField>(resource)) {
        if ((iter = attr_map.find(F_ID)) != attr_map.end()) {
            id_p->SetID(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto cid_p = std::dynamic_pointer_cast<snapshot::CollectionIdField>(resource)) {
        if ((iter = attr_map.find(F_COLLECTON_ID)) != attr_map.end()) {
            cid_p->SetCollectionId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto sid_p = std::dynamic_pointer_cast<snapshot::SchemaIdField>(resource)) {
        if ((iter = attr_map.find(F_SCHEMA_ID)) != attr_map.end()) {
            sid_p->SetSchemaId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto num_p = std::dynamic_pointer_cast<snapshot::NumField>(resource)) {
        if ((iter = attr_map.find(F_NUM)) != attr_map.end()) {
            num_p->SetNum(Str2FieldValue<snapshot::NUM_TYPE>(iter->second));
        }
    }

    if (auto ftype_p = std::dynamic_pointer_cast<snapshot::FtypeField>(resource)) {
        if ((iter = attr_map.find(F_FTYPE)) != attr_map.end()) {
            ftype_p->SetFtype(Str2FieldValue<snapshot::FTYPE_TYPE>(iter->second));
        }
    }

    if (auto fetype_p = std::dynamic_pointer_cast<snapshot::FEtypeField>(resource)) {
        if ((iter = attr_map.find(F_FETYPE)) != attr_map.end()) {
            fetype_p->SetFEtype(Str2FieldValue<snapshot::FETYPE_TYPE>(iter->second));
        }
    }

    if (auto fid_p = std::dynamic_pointer_cast<snapshot::FieldIdField>(resource)) {
        if ((iter = attr_map.find(F_FIELD_ID)) != attr_map.end()) {
            fid_p->SetFieldId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto feid_p = std::dynamic_pointer_cast<snapshot::FieldElementIdField>(resource)) {
        if ((iter = attr_map.find(F_FIELD_ELEMENT_ID)) != attr_map.end()) {
            feid_p->SetFieldElementId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto pid_p = std::dynamic_pointer_cast<snapshot::PartitionIdField>(resource)) {
        if ((iter = attr_map.find(F_PARTITION_ID)) != attr_map.end()) {
            pid_p->SetPartitionId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto pid_p = std::dynamic_pointer_cast<snapshot::SegmentIdField>(resource)) {
        if ((iter = attr_map.find(F_SEGMENT_ID)) != attr_map.end()) {
            pid_p->SetSegmentId(Str2FieldValue<snapshot::ID_TYPE>(iter->second));
        }
    }

    if (auto name_p = std::dynamic_pointer_cast<snapshot::NameField>(resource)) {
        if ((iter = attr_map.find(F_NAME)) != attr_map.end()) {
            name_p->SetName(Str2FieldValue<std::string>(iter->second));
        }
    }

    if (auto pf_p = std::dynamic_pointer_cast<snapshot::ParamsField>(resource)) {
        if ((iter = attr_map.find(F_PARAMS)) != attr_map.end()) {
            pf_p->SetParams(Str2FieldValue<json>(iter->second));
        }
    }

    if (auto size_p = std::dynamic_pointer_cast<snapshot::SizeField>(resource)) {
        if ((iter = attr_map.find(F_SIZE)) != attr_map.end()) {
            size_p->SetSize(Str2FieldValue<snapshot::SIZE_TYPE>(iter->second));
        }
    }

    if (auto rc_p = std::dynamic_pointer_cast<snapshot::RowCountField>(resource)) {
        if ((iter = attr_map.find(F_ROW_COUNT)) != attr_map.end()) {
            rc_p->SetRowCount(Str2FieldValue<snapshot::SIZE_TYPE>(iter->second));
        }
    }

    if (auto tn_p = std::dynamic_pointer_cast<snapshot::TypeNameField>(resource)) {
        if ((iter = attr_map.find(F_TYPE_NAME)) != attr_map.end()) {
            tn_p->SetTypeName(Str2FieldValue<std::string>(iter->second));
        }
    }

    return Status::OK();
}

}  // namespace milvus::engine::meta
