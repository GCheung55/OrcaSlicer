#ifndef slic3r_ExtrusionOrderOptimizer_hpp_
#define slic3r_ExtrusionOrderOptimizer_hpp_

#include "libslic3r.h"
#include <vector>
#include <string>

namespace Slic3r {

class ExtrusionEntityCollection;
enum ExtrusionRole : uint8_t; // Forward declaration from ExtrusionEntity.hpp
class PrintConfig; // Forward declaration for config access (changed from ConfigBase)

// Applies a custom extrusion order to the given collection.
void apply_custom_extrusion_order(
    ExtrusionEntityCollection& collection,
    const std::vector<ExtrusionRole>& custom_order_roles,
    bool preserve_suborder_and_disable_slicer_sort
);

// Helper to get the custom order from config and convert string roles to enum roles
std::vector<ExtrusionRole> get_custom_extrusion_roles_from_config(const PrintConfig& config);

} // namespace Slic3r

#endif // slic3r_ExtrusionOrderOptimizer_hpp_
