#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

using ModuleGetter = std::function< pybind11::module & (std::string const &) >;

void bind_handlegraph_types(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_metadata(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_bdsg_internal_binder_hook_bind(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_mutable_path_metadata(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_mutable_path_mutable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_graph_proxy(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_graph_proxy_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_wang_hash(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mapped_structs(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mapped_structs_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_hash_map(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_expanding_overlay_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_strand_split_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_is_single_stranded(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_eades_algorithm(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_base_packed_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_position_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_packed_path_position_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_packed_reference_path_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_path_position_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_path_subgraph_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_reference_path_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_vectorizable_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_trivially_serializable(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_packed_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_snarl_distance_index(std::function< pybind11::module &(std::string const &namespace_) > &M);


PYBIND11_MODULE(bdsg, root_module) {
	root_module.doc() = "bdsg module";

	std::map <std::string, pybind11::module> modules;
	ModuleGetter M = [&](std::string const &namespace_) -> pybind11::module & {
		auto it = modules.find(namespace_);
		if( it == modules.end() ) throw std::runtime_error("Attempt to access pybind11::module for namespace " + namespace_ + " before it was created!!!");
		return it->second;
	};

	modules[""] = root_module;

	static std::vector<std::string> const reserved_python_words {"nonlocal", "global", };

	auto mangle_namespace_name(
		[](std::string const &ns) -> std::string {
			if ( std::find(reserved_python_words.begin(), reserved_python_words.end(), ns) == reserved_python_words.end() ) return ns;
			return ns+'_';
		}
	);

	std::vector< std::pair<std::string, std::string> > sub_modules {
		{"", "bdsg"},
		{"bdsg", "algorithms"},
		{"", "handlegraph"},
		{"", "std"},
	};
	for(auto &p : sub_modules ) modules[ p.first.empty() ? p.second :  p.first+"::"+p.second ] = modules[p.first].def_submodule( mangle_namespace_name(p.second).c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() );

	//pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

	bind_handlegraph_types(M);
	bind_handlegraph_handle_graph(M);
	bind_handlegraph_path_metadata(M);
	bind_handlegraph_path_handle_graph(M);
	bind_std_bdsg_internal_binder_hook_bind(M);
	bind_handlegraph_mutable_path_metadata(M);
	bind_handlegraph_mutable_path_mutable_handle_graph(M);
	bind_bdsg_graph_proxy(M);
	bind_bdsg_graph_proxy_1(M);
	bind_bdsg_internal_wang_hash(M);
	bind_bdsg_internal_mapped_structs(M);
	bind_bdsg_internal_mapped_structs_1(M);
	bind_bdsg_internal_hash_map(M);
	bind_handlegraph_expanding_overlay_graph(M);
	bind_bdsg_overlays_strand_split_overlay(M);
	bind_bdsg_internal_is_single_stranded(M);
	bind_bdsg_internal_eades_algorithm(M);
	bind_bdsg_internal_base_packed_graph(M);
	bind_handlegraph_path_position_handle_graph(M);
	bind_bdsg_overlays_packed_path_position_overlay(M);
	bind_bdsg_overlays_packed_reference_path_overlay(M);
	bind_bdsg_overlays_path_position_overlays(M);
	bind_bdsg_overlays_path_subgraph_overlay(M);
	bind_bdsg_overlays_reference_path_overlay(M);
	bind_bdsg_overlays_vectorizable_overlays(M);
	bind_handlegraph_trivially_serializable(M);
	bind_bdsg_packed_graph(M);
	bind_bdsg_snarl_distance_index(M);

}
