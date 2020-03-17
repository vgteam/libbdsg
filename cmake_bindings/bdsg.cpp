#include <map>
#include <memory>
#include <stdexcept>
#include <functional>
#include <string>

#include <pybind11/pybind11.h>

typedef std::function< pybind11::module & (std::string const &) > ModuleGetter;

void bind_std_postypes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_locale_classes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_ostream_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_types(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_is_single_stranded(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_mutable_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_wang_hash(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_packed_structs(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_utility(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_varint(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_varint_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_odgi(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_expanding_overlay_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_split_strand_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_handlegraph_path_position_handle_graph(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_packed_path_position_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_path_position_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bdsg_vectorizable_overlays(std::function< pybind11::module &(std::string const &namespace_) > &M);


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
		{"bdsg", "msbvarint"},
		{"bdsg", "sqvarint"},
		{"", "handlegraph"},
		{"", "std"},
	};
	for(auto &p : sub_modules ) modules[p.first.size() ? p.first+"::"+p.second : p.second] = modules[p.first].def_submodule(p.second.c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() );

	//pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

	bind_std_postypes(M);
	bind_std_locale_classes(M);
	bind_std_ostream_tcc(M);
	bind_handlegraph_types(M);
	bind_handlegraph_handle_graph(M);
	bind_bdsg_is_single_stranded(M);
	bind_handlegraph_path_handle_graph(M);
	bind_handlegraph_mutable_handle_graph(M);
	bind_bdsg_wang_hash(M);
	bind_bdsg_packed_structs(M);
	bind_bdsg_utility(M);
	bind_bdsg_varint(M);
	bind_bdsg_varint_1(M);
	bind_bdsg_odgi(M);
	bind_handlegraph_expanding_overlay_graph(M);
	bind_bdsg_split_strand_graph(M);
	bind_handlegraph_path_position_handle_graph(M);
	bind_bdsg_packed_path_position_overlays(M);
	bind_bdsg_path_position_overlays(M);
	bind_bdsg_vectorizable_overlays(M);

}
