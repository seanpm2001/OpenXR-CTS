// Copyright (c) 2019-2023, The Khronos Group Inc.
// Copyright (c) Meta Platforms, LLC and its affiliates. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "utils.h"
#include "conformance_utils.h"
#include "conformance_framework.h"
#include <catch2/catch_test_macros.hpp>
#include "throw_helpers.h"

#include <openxr/openxr.h>

namespace Conformance
{
    static inline XrUuidEXT queryHeadsetId(XrInstance instance, XrSystemId systemId)
    {
        XrSystemHeadsetIdPropertiesMETA headsetId = {XR_TYPE_SYSTEM_HEADSET_ID_PROPERTIES_META};
        XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
        systemProperties.next = &headsetId;

        REQUIRE(XR_SUCCESS == xrGetSystemProperties(instance, systemId, &systemProperties));
        return headsetId.id;
    }

    TEST_CASE("XR_META_headset_id", "")
    {
        GlobalData& globalData = GetGlobalData();
        if (!globalData.IsInstanceExtensionSupported(XR_META_HEADSET_ID_EXTENSION_NAME)) {
            return;
        }

        SECTION("Extension not enabled")
        {
            // validate that the extension has not been force enabled...
            if (!globalData.enabledInstanceExtensionNames.contains(XR_META_HEADSET_ID_EXTENSION_NAME)) {
                AutoBasicInstance instance;

                XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO, nullptr, globalData.options.formFactorValue};
                XrSystemId systemId = XR_NULL_SYSTEM_ID;
                REQUIRE(XR_SUCCESS == xrGetSystem(instance, &systemGetInfo, &systemId));

                XrUuidEXT headsetId = queryHeadsetId(instance, systemId);

                // Validate headsetid has NOT been filled in
                XrUuidEXT empty{};
                REQUIRE(memcmp(&empty, &headsetId, sizeof(XrUuidEXT)) == 0);
            }
        }

        SECTION("xrGetSystemProperties", "")
        {
            AutoBasicInstance instance({XR_META_HEADSET_ID_EXTENSION_NAME});

            XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO, nullptr, globalData.options.formFactorValue};
            XrSystemId systemId = XR_NULL_SYSTEM_ID;
            REQUIRE(XR_SUCCESS == xrGetSystem(instance, &systemGetInfo, &systemId));

            SECTION("Valid UUID returned")
            {
                XrUuidEXT headsetId = queryHeadsetId(instance, systemId);

                // Validate headsetid has been filled in
                XrUuidEXT empty{};
                REQUIRE(memcmp(&empty, &headsetId, sizeof(XrUuidEXT)) != 0);
            }

            SECTION("Consistent UUID returned")
            {
                XrUuidEXT headsetId1 = queryHeadsetId(instance, systemId);
                XrUuidEXT headsetId2 = queryHeadsetId(instance, systemId);

                // Validate headsetid is consistent
                REQUIRE(memcmp(&headsetId1, &headsetId2, sizeof(XrUuidEXT)) == 0);
            }
        }
    }

}  // namespace Conformance
