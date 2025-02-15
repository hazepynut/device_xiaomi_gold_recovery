#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include <android-base/properties.h>
#include <android-base/logging.h>
#include <sys/resource.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;

void property_override(const std::string& name, const std::string& value) {
    prop_info* pi = (prop_info*)__system_property_find(name.c_str());
    if (pi) {
        __system_property_update(pi, value.c_str(), value.size());
    } else if (__system_property_add(name.c_str(), name.size(), value.c_str(), value.size()) < 0) {
        LOG(ERROR) << "property_set(\"" << name << "\", \"" << value << "\") failed: "
                   << "__system_property_add failed";
    }
}

void model_property_override(const std::string& device, const std::string& model) {
    static const std::vector<std::string> prop_suffixes = {
        "", "odm", "system", "vendor", "product", "system_ext"
    };

    for (const auto& suffix : prop_suffixes) {
        std::string prefix = "ro.product." + suffix;
        if (!suffix.empty()) {
            property_override(prefix + ".device", device);
            property_override(prefix + ".name", device);
            property_override(prefix + ".model", model);
        }
    }

    property_override("ro.build.product", device);
}

void vendor_load_properties() {
    const std::string sku = GetProperty("ro.boot.product.hardware.sku", "");

    const std::unordered_map<std::string, std::string> sku_map = {
        {"iron_gl", "Redmi Note 13 5G"},
        {"iron_p_gl", "Redmi Note 13 5G"},
        {"gold_a_cn", "Redmi Note 13R Pro"},
        {"gold_a_in", "POCO X6 Neo 5G"},
        {"gold_a_old", "Redmi Note 13R Pro"},
        {"gold_cn", "Redmi Note 13 5G"},
        {"gold_in", "Redmi Note 13 5G"}
    };

    auto it = sku_map.find(sku);
    model_property_override("gold", it != sku_map.end() ? it->second : "Redmi Note 13 5G");
}