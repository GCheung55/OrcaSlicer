#include "ExtrusionOrderOptimizer.hpp"
#include "ExtrusionEntityCollection.hpp"
#include "ExtrusionEntity.hpp" // For ExtrusionRole enum and ExtrusionEntity::string_to_role
#include "PrintConfig.hpp"     // Defines PrintConfig
#include "Config.hpp"          // Defines DynamicConfig and ConfigOptionStrings
#include <algorithm>           // For std::stable_sort, std::remove_if
#include <map>                 // For mapping roles to priorities

namespace Slic3r {

std::vector<ExtrusionRole> get_custom_extrusion_roles_from_config(const PrintConfig& config) {
    std::vector<ExtrusionRole> roles_enum_list;
    if (config.has("custom_extrusion_role_order")) {
        const Slic3r::ConfigOption* opt_base = config.option("custom_extrusion_role_order");
        const Slic3r::ConfigOptionStrings* opt_strings = nullptr;
        if (opt_base) {
            opt_strings = dynamic_cast<const Slic3r::ConfigOptionStrings*>(opt_base);
        }
        if (opt_strings) {
            const auto& role_strings = opt_strings->values;
            for (const std::string& role_str : role_strings) {
                ExtrusionRole role = ExtrusionEntity::string_to_role(role_str);
            if (role != erNone && role != erMixed && role != erCount) { // Ensure valid, concrete roles
                roles_enum_list.push_back(role);
            }
            }
        }
    }
    return roles_enum_list;
}

void apply_custom_extrusion_order(
    ExtrusionEntityCollection& collection,
    const std::vector<ExtrusionRole>& custom_order_roles,
    bool preserve_suborder_and_disable_slicer_sort)
{
    if (custom_order_roles.empty() || collection.entities.empty()) {
        // No custom order defined or collection is empty, nothing to do.
        // Or, if we want Slic3r's default sort to run, ensure no_sort is false.
        if (!preserve_suborder_and_disable_slicer_sort) {
            collection.no_sort = false;
        }
        return;
    }

    // Create a map for role priorities based on the custom_order_roles vector
    // Roles earlier in the vector get lower (higher) priority values.
    std::map<ExtrusionRole, int> role_priority;
    for (size_t i = 0; i < custom_order_roles.size(); ++i) {
        role_priority[custom_order_roles[i]] = static_cast<int>(i);
    }

    // Max priority for roles not in custom_order_roles, to place them at the end.
    const int unlisted_role_priority = static_cast<int>(custom_order_roles.size());

    std::stable_sort(collection.entities.begin(), collection.entities.end(),
        [&](const ExtrusionEntity* a, const ExtrusionEntity* b) {
        ExtrusionRole role_a = a->role();
        ExtrusionRole role_b = b->role();

        int priority_a = unlisted_role_priority;
        auto it_a = role_priority.find(role_a);
        if (it_a != role_priority.end()) {
            priority_a = it_a->second;
        }

        int priority_b = unlisted_role_priority;
        auto it_b = role_priority.find(role_b);
        if (it_b != role_priority.end()) {
            priority_b = it_b->second;
        }

        return priority_a < priority_b;
    });

    if (preserve_suborder_and_disable_slicer_sort) {
        collection.no_sort = true;
    } else {
        collection.no_sort = false; // Allow Slic3r's path optimization if desired
    }
}

} // namespace Slic3r
