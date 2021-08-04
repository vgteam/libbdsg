#include <bdsg/snarl_distance_index.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/snarl_decomposition.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <functional>
#include <string>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>
#include <bdsg/internal/binder_hook_compile.hpp>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>);
#endif

// bdsg::SnarlDistanceIndex file:bdsg/snarl_distance_index.hpp line:96
struct PyCallBack_bdsg_SnarlDistanceIndex : public bdsg::SnarlDistanceIndex {
	using bdsg::SnarlDistanceIndex::SnarlDistanceIndex;

	struct handlegraph::net_handle_t get_root() const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_root();
	}
	bool is_root(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_root(a0);
	}
	bool is_snarl(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_snarl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_snarl(a0);
	}
	bool is_chain(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_chain");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_chain(a0);
	}
	bool is_node(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_node(a0);
	}
	bool is_sentinel(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_sentinel");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_sentinel(a0);
	}
	struct handlegraph::net_handle_t get_net(const struct handlegraph::handle_t & a0, const class handlegraph::HandleGraph * a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_net");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_net(a0, a1);
	}
	struct handlegraph::handle_t get_handle(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_handle(a0, a1);
	}
	struct handlegraph::net_handle_t get_parent(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_parent");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_parent(a0);
	}
	struct handlegraph::net_handle_t get_bound(const struct handlegraph::net_handle_t & a0, bool a1, bool a2) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_bound");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_bound(a0, a1, a2);
	}
	struct handlegraph::net_handle_t flip(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::flip(a0);
	}
	struct handlegraph::net_handle_t canonical(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "canonical");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::canonical(a0);
	}
	enum handlegraph::SnarlDecomposition::endpoint_t starts_at(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "starts_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::overload_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		return SnarlDistanceIndex::starts_at(a0);
	}
	enum handlegraph::SnarlDecomposition::endpoint_t ends_at(const struct handlegraph::net_handle_t & a0) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "ends_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::overload_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		return SnarlDistanceIndex::ends_at(a0);
	}
	bool for_each_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::for_each_child_impl(a0, a1);
	}
	bool for_each_traversal_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::for_each_traversal_impl(a0, a1);
	}
	bool follow_net_edges_impl(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1, bool a2, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a3) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "follow_net_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2, a3);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::follow_net_edges_impl(a0, a1, a2, a3);
	}
	struct handlegraph::net_handle_t get_parent_traversal(const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_parent_traversal");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::overload_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_parent_traversal(a0, a1);
	}
	bool for_each_tippy_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_tippy_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_tippy_child_impl(a0, a1);
	}
	bool for_each_traversal_start_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_start_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_start_impl(a0, a1);
	}
	bool for_each_traversal_end_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override { 
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_end_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::overload_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_end_impl(a0, a1);
	}
};

void bind_bdsg_snarl_distance_index(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::SnarlDistanceIndex file:bdsg/snarl_distance_index.hpp line:96
		pybind11::class_<bdsg::SnarlDistanceIndex, std::shared_ptr<bdsg::SnarlDistanceIndex>, PyCallBack_bdsg_SnarlDistanceIndex, handlegraph::SnarlDecomposition> cl(M("bdsg"), "SnarlDistanceIndex", "The distance index. Stores minimum distances among nodes in each \n snarl's netgraph and each chain.\n Also used to store the snarl tree");
		{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex file:bdsg/snarl_distance_index.hpp line:1161
			auto & enclosing_class = cl;
			pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex>> cl(enclosing_class, "TemporaryDistanceIndex", "");
			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord file:bdsg/snarl_distance_index.hpp line:1179
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord>> cl(enclosing_class, "TemporaryRecord", "");
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord(o); } ) );
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord(); } ) );
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord file:bdsg/snarl_distance_index.hpp line:1181
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporaryChainRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord(o); } ) );
				cl.def_readwrite("start_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::start_node_id);
				cl.def_readwrite("start_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::start_node_rev);
				cl.def_readwrite("end_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_id);
				cl.def_readwrite("end_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_rev);
				cl.def_readwrite("end_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_length);
				cl.def_readwrite("tree_depth", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::tree_depth);
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::parent);
				cl.def_readwrite("min_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::min_length);
				cl.def_readwrite("max_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::max_length);
				cl.def_readwrite("children", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::children);
				cl.def_readwrite("prefix_sum", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::prefix_sum);
				cl.def_readwrite("forward_loops", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::forward_loops);
				cl.def_readwrite("backward_loops", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::backward_loops);
				cl.def_readwrite("chain_components", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::chain_components);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::rank_in_parent);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::reversed_in_parent);
				cl.def_readwrite("is_trivial", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::is_trivial);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::is_tip);
				cl.def_readwrite("root_snarl_index", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::root_snarl_index);
				cl.def_readwrite("loopable", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::loopable);
				cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::*)() const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length() const --> unsigned long");
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord file:bdsg/snarl_distance_index.hpp line:1208
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporarySnarlRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord(o); } ) );
				cl.def_readwrite("start_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_id);
				cl.def_readwrite("start_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_rev);
				cl.def_readwrite("start_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_length);
				cl.def_readwrite("end_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_id);
				cl.def_readwrite("end_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_rev);
				cl.def_readwrite("end_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_length);
				cl.def_readwrite("node_count", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::node_count);
				cl.def_readwrite("min_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::min_length);
				cl.def_readwrite("max_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::max_length);
				cl.def_readwrite("max_distance", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::max_distance);
				cl.def_readwrite("tree_depth", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::tree_depth);
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::parent);
				cl.def_readwrite("children", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::children);
				cl.def_readwrite("tippy_child_ranks", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::tippy_child_ranks);
				cl.def_readwrite("distances", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::distances);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::rank_in_parent);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::reversed_in_parent);
				cl.def_readwrite("loop_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::loop_start);
				cl.def_readwrite("loop_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::loop_end);
				cl.def_readwrite("is_trivial", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_trivial);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_tip);
				cl.def_readwrite("is_root_snarl", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_root_snarl);
				cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::*)()) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::get_max_record_length() --> unsigned long");
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord file:bdsg/snarl_distance_index.hpp line:1234
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporaryNodeRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord(o); } ) );
				cl.def_readwrite("node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::node_id);
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::parent);
				cl.def_readwrite("node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::node_length);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::rank_in_parent);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::reversed_in_parent);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::is_tip);
				cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::*)()) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::get_max_record_length() --> unsigned long");
			}

			cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex(); } ) );
			cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex(o); } ) );
			cl.def_readwrite("min_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::min_node_id);
			cl.def_readwrite("max_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::max_node_id);
			cl.def_readwrite("root_structure_count", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::root_structure_count);
			cl.def_readwrite("max_tree_depth", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::max_tree_depth);
			cl.def_readwrite("max_index_size", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::max_index_size);
			cl.def_readwrite("max_distance", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::max_distance);
			cl.def_readwrite("components", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::components);
			cl.def_readwrite("root_snarl_components", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::root_snarl_components);
			cl.def_readwrite("temp_chain_records", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::temp_chain_records);
			cl.def_readwrite("temp_snarl_records", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::temp_snarl_records);
			cl.def_readwrite("temp_node_records", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::temp_node_records);
			cl.def("structure_start_end_as_string", (std::string (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::*)(struct std::pair<enum bdsg::SnarlDistanceIndex::temp_record_t, unsigned long>) const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::structure_start_end_as_string, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::structure_start_end_as_string(struct std::pair<enum bdsg::SnarlDistanceIndex::temp_record_t, unsigned long>) const --> std::string", pybind11::arg("index"));
			cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::get_max_record_length() const --> unsigned long");
		}

		cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex(); }, [](){ return new PyCallBack_bdsg_SnarlDistanceIndex(); } ) );

		pybind11::enum_<bdsg::SnarlDistanceIndex::temp_record_t>(cl, "temp_record_t", pybind11::arithmetic(), "")
			.value("TEMP_CHAIN", bdsg::SnarlDistanceIndex::TEMP_CHAIN)
			.value("TEMP_SNARL", bdsg::SnarlDistanceIndex::TEMP_SNARL)
			.value("TEMP_NODE", bdsg::SnarlDistanceIndex::TEMP_NODE)
			.value("TEMP_ROOT", bdsg::SnarlDistanceIndex::TEMP_ROOT)
			.export_values();

		cl.def("get_root", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::get_root, "Get a net handle referring to a tip-to-tip traversal of the contents of the root snarl.\n TODO: Special handling for circular things in the root snarl? Circular traversal type?\n\nC++: bdsg::SnarlDistanceIndex::get_root() const --> struct handlegraph::net_handle_t");
		cl.def("is_root", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_root, "Return true if the given handle refers to (a traversal of) the root\n snarl, and false otherwise.\n\nC++: bdsg::SnarlDistanceIndex::is_root(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_snarl", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_snarl, "Returns true if the given net handle refers to (a traversal of) a snarl.\n\nC++: bdsg::SnarlDistanceIndex::is_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_chain, "Returns true if the given net handle refers to (a traversal of) a chain.\n\nC++: bdsg::SnarlDistanceIndex::is_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_trivial_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_trivial_chain, "Returns true if the given net handle refers to (a traversal of) a trivial chain that represents a single node.\n\nC++: bdsg::SnarlDistanceIndex::is_trivial_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_node", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_node, "Returns true if the given net handle refers to (a traversal of) a single node, and thus has a corresponding handle_t.\n\nC++: bdsg::SnarlDistanceIndex::is_node(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_sentinel", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_sentinel, "Return true if the given net handle is a snarl bound sentinel (in either\n inward or outward orientation), and false otherwise.\n\nC++: bdsg::SnarlDistanceIndex::is_sentinel(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("get_net", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::get_net, "Turn a handle to an oriented node into a net handle for a start-to-end or end-to-start traversal of the node, as appropriate.\n\nC++: bdsg::SnarlDistanceIndex::get_net(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::net_handle_t", pybind11::arg("handle"), pybind11::arg("graph"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::get_handle, "For a net handle to a traversal of a single node, get the handle for that node in the orientation it is traversed.\n May not be called for other net handles.\n\nC++: bdsg::SnarlDistanceIndex::get_handle(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::handle_t", pybind11::arg("net"), pybind11::arg("graph"));
		cl.def("get_parent", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_parent, "Get the parent snarl of a chain, or the parent chain of a snarl or node.\n If the child is start-to-end or end-to-start, and the parent is a chain,\n the chain comes out facing the same way, accounting for the relative\n orientation of the child snarl or node in the chain. Otherwise,\n everything is produced as start-to-end, even if that is not actually a\n realizable traversal of a snarl or chain. May not be called on the root\n snarl.\n\n Also works on snarl boundary sentinels.\n\nC++: bdsg::SnarlDistanceIndex::get_parent(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("child"));
		cl.def("get_bound", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, bool, bool) const) &bdsg::SnarlDistanceIndex::get_bound, "Get the bounding handle for the snarl or chain referenced by the given\n net handle, getting the start or end facing in or out as appropriate.\n\n For snarls, returns the bounding sentinel net handles. For chains,\n returns net handles for traversals of the bounding nodes of the chain.\n\n Ignores traversal type.\n\n May not be called on traversals of individual nodes.\n\nC++: bdsg::SnarlDistanceIndex::get_bound(const struct handlegraph::net_handle_t &, bool, bool) const --> struct handlegraph::net_handle_t", pybind11::arg("snarl"), pybind11::arg("get_end"), pybind11::arg("face_in"));
		cl.def("flip", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::flip, "Return a net handle to the same snarl/chain/node in the opposite orientation.\n No effect on tip-to-tip, start-to-start, or end-to-end net handles. Flips all the others.\n\nC++: bdsg::SnarlDistanceIndex::flip(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("canonical", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::canonical, "Get a canonical traversal handle from any net handle. All handles to the\n same net graph element have the same canonical traversal. That canonical\n traversal must be realizable, and might not always be start-to-end or\n even consistently be the same kind of traversal for different snarls,\n chains, or nodes. Mostly useful to normalize for equality comparisons.\n\nC++: bdsg::SnarlDistanceIndex::canonical(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("starts_at", (enum handlegraph::SnarlDecomposition::endpoint_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::starts_at, "Return the kind of location at which the given traversal starts.\n\nC++: bdsg::SnarlDistanceIndex::starts_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("ends_at", (enum handlegraph::SnarlDecomposition::endpoint_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::ends_at, "Return the kind of location at which the given traversal ends.\n\nC++: bdsg::SnarlDistanceIndex::ends_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("get_parent_traversal", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_parent_traversal, "Get a net handle for traversals of a snarl or chain that contains\n the given oriented bounding node traversals or sentinels. Given two\n sentinels for a snarl, produces a net handle to a start-to-end,\n end-to-end, end-to-start, or start-to-start traversal of that snarl.\n Given handles to traversals of the bounding nodes of a chain, similarly\n produces a net handle to a traversal of the chain.\n\n For a chain, either or both handles can also be a snarl containing tips,\n for a tip-to-start, tip-to-end, start-to-tip, end-to-tip, or tip-to-tip\n traversal. Similarly, for a snarl, either or both handles can be a chain\n in the snarl that contains internal tips, or that has no edges on the\n appropriate end.\n\n May only be called if a path actually exists between the given start\n and end.\n\nC++: bdsg::SnarlDistanceIndex::get_parent_traversal(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("traversal_start"), pybind11::arg("traversal_end"));
		cl.def("load", (void (bdsg::SnarlDistanceIndex::*)(std::string &)) &bdsg::SnarlDistanceIndex::load, "C++: bdsg::SnarlDistanceIndex::load(std::string &) --> void", pybind11::arg("filename"));
		cl.def("load", (void (bdsg::SnarlDistanceIndex::*)(int)) &bdsg::SnarlDistanceIndex::load, "C++: bdsg::SnarlDistanceIndex::load(int) --> void", pybind11::arg("fd"));
		cl.def("save", (void (bdsg::SnarlDistanceIndex::*)(std::string &) const) &bdsg::SnarlDistanceIndex::save, "C++: bdsg::SnarlDistanceIndex::save(std::string &) const --> void", pybind11::arg("filename"));
		cl.def("save", (void (bdsg::SnarlDistanceIndex::*)(int)) &bdsg::SnarlDistanceIndex::save, "C++: bdsg::SnarlDistanceIndex::save(int) --> void", pybind11::arg("fd"));
		cl.def("distance_in_parent", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1, const struct handlegraph::net_handle_t & a2) -> unsigned long { return o.distance_in_parent(a0, a1, a2); }, "", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"));
		cl.def("distance_in_parent", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::distance_in_parent, "Find the distance between the two child node sides in the parent, facing each other, not \n including the lengths of the nodes\n This only takes into account the endpoint of the net_handle_t traversal, it does not care if the traversal\n was possible. Doesn't allow you to find the distance from a traversal ending/starting in a tip\n requires that the children are children of the parent\n For chains, the distance to/from a snarl is really the distance from the outer node of the snarl\n Returns std::numeric_limits<size_t>::max() if there is not path between them in the parent \n or if they are not children of the parent\n\nC++: bdsg::SnarlDistanceIndex::distance_in_parent(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const --> unsigned long", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"), pybind11::arg("graph"));
		cl.def("lowest_common_ancestor", (struct std::pair<struct handlegraph::net_handle_t, bool> (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::lowest_common_ancestor, "For two net handles, get a net handle lowest common ancestor\n If the lowest common ancestor is the root, then the two handles may be in \n different connected components. In this case, return false\n\nC++: bdsg::SnarlDistanceIndex::lowest_common_ancestor(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> struct std::pair<struct handlegraph::net_handle_t, bool>", pybind11::arg("net1"), pybind11::arg("net2"));
		cl.def("node_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::node_length, "Return the length of the net, which must represent a node (or sentinel of a snarl)\n\nC++: bdsg::SnarlDistanceIndex::node_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("minimum_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::minimum_length, "This is also the length of a net, but it can also be a snarl or chain. \n The length of a chain includes the boundary nodes, a snarl does not\n A looping chain only includes the start/end node once\n\nC++: bdsg::SnarlDistanceIndex::minimum_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("node_id", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::node_id, "What is the node id of the node represented by this net handle\n net must be a node or a sentinel\n\nC++: bdsg::SnarlDistanceIndex::node_id(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("has_connectivity", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const) &bdsg::SnarlDistanceIndex::has_connectivity, "C++: bdsg::SnarlDistanceIndex::has_connectivity(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const --> bool", pybind11::arg("net"), pybind11::arg("start"), pybind11::arg("end"));
		cl.def("has_external_connectivity", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const) &bdsg::SnarlDistanceIndex::has_external_connectivity, "C++: bdsg::SnarlDistanceIndex::has_external_connectivity(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const --> bool", pybind11::arg("net"), pybind11::arg("start"), pybind11::arg("end"));
		cl.def("minimum_distance", [](bdsg::SnarlDistanceIndex const &o, long long const & a0, bool const & a1, unsigned long const & a2, long long const & a3, bool const & a4, unsigned long const & a5) -> unsigned long { return o.minimum_distance(a0, a1, a2, a3, a4, a5); }, "", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"));
		cl.def("minimum_distance", [](bdsg::SnarlDistanceIndex const &o, long long const & a0, bool const & a1, unsigned long const & a2, long long const & a3, bool const & a4, unsigned long const & a5, bool const & a6) -> unsigned long { return o.minimum_distance(a0, a1, a2, a3, a4, a5, a6); }, "", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"), pybind11::arg("unoriented_distance"));
		cl.def("minimum_distance", (unsigned long (bdsg::SnarlDistanceIndex::*)(long long, bool, unsigned long, long long, bool, unsigned long, bool, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::minimum_distance, "Get the minimum distance between two positions in the graph\n If unoriented_distance is true, then ignore the orientations of the positions\n Otherwise, distance is calculated from the first position going forward to the second position going forward\n The distance includes one of the positions; the distance from one position to itself is 1\n Returns std::numeric_limits<size_t>::max() if there is no path between the two positions\n\nC++: bdsg::SnarlDistanceIndex::minimum_distance(long long, bool, unsigned long, long long, bool, unsigned long, bool, const class handlegraph::HandleGraph *) const --> unsigned long", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"), pybind11::arg("unoriented_distance"), pybind11::arg("graph"));
		cl.def("print_self", (void (bdsg::SnarlDistanceIndex::*)()) &bdsg::SnarlDistanceIndex::print_self, "C++: bdsg::SnarlDistanceIndex::print_self() --> void");
		cl.def("set_size_limit", (void (bdsg::SnarlDistanceIndex::*)(unsigned long)) &bdsg::SnarlDistanceIndex::set_size_limit, "//////////////  Construction methods\n\nC++: bdsg::SnarlDistanceIndex::set_size_limit(unsigned long) --> void", pybind11::arg("size"));
		cl.def("net_handle_as_string", (std::string (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::net_handle_as_string, "C++: bdsg::SnarlDistanceIndex::net_handle_as_string(const struct handlegraph::net_handle_t &) const --> std::string", pybind11::arg("net"));
		cl.def_static("sum", (unsigned long (*)(const class std::vector<unsigned long>)) &bdsg::SnarlDistanceIndex::sum, "Add integers, returning max() if any of them are max()\n\nC++: bdsg::SnarlDistanceIndex::sum(const class std::vector<unsigned long>) --> unsigned long", pybind11::arg("vals"));
		cl.def_static("minus", (unsigned long (*)(unsigned long, unsigned long)) &bdsg::SnarlDistanceIndex::minus, "C++: bdsg::SnarlDistanceIndex::minus(unsigned long, unsigned long) --> unsigned long", pybind11::arg("x"), pybind11::arg("y"));
		cl.def_static("bit_width", (unsigned long (*)(unsigned long)) &bdsg::SnarlDistanceIndex::bit_width, "C++: bdsg::SnarlDistanceIndex::bit_width(unsigned long) --> unsigned long", pybind11::arg("value"));
	}
}
