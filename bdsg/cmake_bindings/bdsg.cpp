#include <map>
#include <memory>
#include <stdexcept>
#include <functional>
#include <string>

#include <pybind11/pybind11.h>

typedef std::function< pybind11::module & (std::string const &) > ModuleGetter;

void bind_handlegraph_types(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_mutable_path_mutable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_wang_hash(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mapped_structs(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mapped_structs_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mapped_structs_2(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_utility(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_expanding_overlay_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_strand_split_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_is_single_stranded(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_eades_algorithm(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_graph_proxy(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_graph_proxy_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_bdsg_internal_binder_hook_bind(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_trivially_serializable(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_internal_mmap_backend(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_odgi(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_position_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_packed_path_position_overlay(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_packed_path_position_overlay_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_path_position_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_vectorizable_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_overlays_vectorizable_overlays_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_snarl_decomposition(std::function< pybind11::module &(std::string const &namespace_) > &M);
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

	std::vector< std::pair<std::string, std::string> > sub_modules {
		{"", "bdsg"},
		{"bdsg", "algorithms"},
		{"bdsg", "yomo"},
		{"", "handlegraph"},
		{"", "std"},
	};
	for(auto &p : sub_modules ) modules[p.first.size() ? p.first+"::"+p.second : p.second] = modules[p.first].def_submodule(p.second.c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() );

	//pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

	bind_handlegraph_types(M);
	bind_handlegraph_handle_graph(M);
	bind_handlegraph_path_handle_graph(M);
	bind_handlegraph_mutable_path_mutable_handle_graph(M);
	bind_bdsg_internal_wang_hash(M);
	bind_bdsg_internal_mapped_structs(M);
	bind_bdsg_internal_mapped_structs_1(M);
	bind_bdsg_internal_mapped_structs_2(M);
	bind_bdsg_internal_utility(M);
	bind_handlegraph_expanding_overlay_graph(M);
	bind_bdsg_overlays_strand_split_overlay(M);
	bind_bdsg_internal_is_single_stranded(M);
	bind_bdsg_internal_eades_algorithm(M);
	bind_bdsg_internal_graph_proxy(M);
	bind_bdsg_internal_graph_proxy_1(M);
	bind_std_bdsg_internal_binder_hook_bind(M);
	bind_handlegraph_trivially_serializable(M);
	bind_bdsg_internal_mmap_backend(M);
	bind_bdsg_odgi(M);
	bind_handlegraph_path_position_handle_graph(M);
	bind_bdsg_overlays_packed_path_position_overlay(M);
	bind_bdsg_overlays_packed_path_position_overlay_1(M);
	bind_bdsg_overlays_path_position_overlays(M);
	bind_bdsg_overlays_vectorizable_overlays(M);
	bind_bdsg_overlays_vectorizable_overlays_1(M);
	bind_handlegraph_snarl_decomposition(M);
	bind_bdsg_snarl_distance_index(M);

}
