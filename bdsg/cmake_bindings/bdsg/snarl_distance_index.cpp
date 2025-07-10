#include <bdsg/snarl_distance_index.hpp>
#include <functional>
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/snarl_decomposition.hpp>
#include <handlegraph/trivially_serializable.hpp>
#include <handlegraph/types.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <tuple>
#include <utility>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <bdsg/internal/binder_hook_compile.hpp>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <fstream>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

// bdsg::SnarlDistanceIndex file:bdsg/snarl_distance_index.hpp line:181
struct PyCallBack_bdsg_SnarlDistanceIndex : public bdsg::SnarlDistanceIndex {
	using bdsg::SnarlDistanceIndex::SnarlDistanceIndex;

	void dissociate() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "dissociate");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return SnarlDistanceIndex::dissociate();
	}
	void serialize(const class std::function<void (const void *, unsigned long)> & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "serialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return SnarlDistanceIndex::serialize(a0);
	}
	void serialize(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "serialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return SnarlDistanceIndex::serialize(a0);
	}
	void deserialize(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "deserialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return SnarlDistanceIndex::deserialize(a0);
	}
	unsigned int get_magic_number() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_magic_number");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<unsigned int>::value) {
				static pybind11::detail::override_caster_t<unsigned int> caster;
				return pybind11::detail::cast_ref<unsigned int>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<unsigned int>(std::move(o));
		}
		return SnarlDistanceIndex::get_magic_number();
	}
	struct handlegraph::net_handle_t get_root() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_root();
	}
	bool is_root(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_root");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_root(a0);
	}
	bool is_snarl(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_snarl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_snarl(a0);
	}
	bool is_chain(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_chain");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_chain(a0);
	}
	bool is_node(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_node");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_node(a0);
	}
	bool is_sentinel(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "is_sentinel");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::is_sentinel(a0);
	}
	struct handlegraph::net_handle_t get_net(const struct handlegraph::handle_t & a0, const class handlegraph::HandleGraph * a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_net");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_net(a0, a1);
	}
	struct handlegraph::handle_t get_handle(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_handle");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_handle(a0, a1);
	}
	struct handlegraph::net_handle_t get_parent(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_parent");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_parent(a0);
	}
	struct handlegraph::net_handle_t get_bound(const struct handlegraph::net_handle_t & a0, bool a1, bool a2) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_bound");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_bound(a0, a1, a2);
	}
	struct handlegraph::net_handle_t flip(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "flip");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::flip(a0);
	}
	struct handlegraph::net_handle_t canonical(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "canonical");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::canonical(a0);
	}
	enum handlegraph::SnarlDecomposition::endpoint_t starts_at(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "starts_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::override_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		return SnarlDistanceIndex::starts_at(a0);
	}
	enum handlegraph::SnarlDecomposition::endpoint_t ends_at(const struct handlegraph::net_handle_t & a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "ends_at");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<enum handlegraph::SnarlDecomposition::endpoint_t>::value) {
				static pybind11::detail::override_caster_t<enum handlegraph::SnarlDecomposition::endpoint_t> caster;
				return pybind11::detail::cast_ref<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<enum handlegraph::SnarlDecomposition::endpoint_t>(std::move(o));
		}
		return SnarlDistanceIndex::ends_at(a0);
	}
	bool for_each_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::for_each_child_impl(a0, a1);
	}
	bool for_each_traversal_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::for_each_traversal_impl(a0, a1);
	}
	bool follow_net_edges_impl(const struct handlegraph::net_handle_t & a0, const class handlegraph::HandleGraph * a1, bool a2, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a3) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "follow_net_edges_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1, a2, a3);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDistanceIndex::follow_net_edges_impl(a0, a1, a2, a3);
	}
	struct handlegraph::net_handle_t get_parent_traversal(const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "get_parent_traversal");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<struct handlegraph::net_handle_t>::value) {
				static pybind11::detail::override_caster_t<struct handlegraph::net_handle_t> caster;
				return pybind11::detail::cast_ref<struct handlegraph::net_handle_t>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<struct handlegraph::net_handle_t>(std::move(o));
		}
		return SnarlDistanceIndex::get_parent_traversal(a0, a1);
	}
	bool for_each_tippy_child_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_tippy_child_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_tippy_child_impl(a0, a1);
	}
	bool for_each_traversal_start_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_start_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_start_impl(a0, a1);
	}
	bool for_each_traversal_end_impl(const struct handlegraph::net_handle_t & a0, const class std::function<bool (const struct handlegraph::net_handle_t &)> & a1) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "for_each_traversal_end_impl");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		return SnarlDecomposition::for_each_traversal_end_impl(a0, a1);
	}
	void serialize(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "serialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return TriviallySerializable::serialize(a0);
	}
	void deserialize(const std::string & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const bdsg::SnarlDistanceIndex *>(this), "deserialize");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return TriviallySerializable::deserialize(a0);
	}
};

void bind_bdsg_snarl_distance_index(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // bdsg::SnarlDistanceIndex file:bdsg/snarl_distance_index.hpp line:181
		pybind11::class_<bdsg::SnarlDistanceIndex, std::shared_ptr<bdsg::SnarlDistanceIndex>, PyCallBack_bdsg_SnarlDistanceIndex, handlegraph::SnarlDecomposition, handlegraph::TriviallySerializable> cl(M("bdsg"), "SnarlDistanceIndex", "The distance index, which also acts as a snarl decomposition.\n\n The distance index provides an interface to traverse the snarl tree and to\n find minimum distances between two sibling nodes in the snarl tree (eg\n between two chains that are children of the same snarl).\n\n It also provides a method for quickly calculating the minimum distance\n between two positions on the graph.\n\n The implementation here is tightly coupled with the filling-in code in vg\n (see vg::fill_in_distance_index()). To make a SnarlDistanceIndex that\n actually works, you have to construct the object, and then call\n get_snarl_tree_records() with zero or more TemporaryDistanceIndex objects\n for connected components, and a graph.\n\n The TemporaryDistanceIndex needs to have a variety of TemporaryRecord\n implementation classes (TemporaryChainRecord, TemporarySnarlRecord,\n TemporaryNodeRecord) set up and added to it; this all has to be done \"by\n hand\", as it were, because no code is in this library to help you do it.\n\n  ");
		cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex(); }, [](){ return new PyCallBack_bdsg_SnarlDistanceIndex(); } ) );

		pybind11::enum_<bdsg::SnarlDistanceIndex::connectivity_t>(cl, "connectivity_t", pybind11::arithmetic(), "The connectivity of a net_handle- this defines the direction that the net_handle is traversed")
			.value("START_START", bdsg::SnarlDistanceIndex::START_START)
			.value("START_END", bdsg::SnarlDistanceIndex::START_END)
			.value("START_TIP", bdsg::SnarlDistanceIndex::START_TIP)
			.value("END_START", bdsg::SnarlDistanceIndex::END_START)
			.value("END_END", bdsg::SnarlDistanceIndex::END_END)
			.value("END_TIP", bdsg::SnarlDistanceIndex::END_TIP)
			.value("TIP_START", bdsg::SnarlDistanceIndex::TIP_START)
			.value("TIP_END", bdsg::SnarlDistanceIndex::TIP_END)
			.value("TIP_TIP", bdsg::SnarlDistanceIndex::TIP_TIP)
			.export_values();


		pybind11::enum_<bdsg::SnarlDistanceIndex::net_handle_record_t>(cl, "net_handle_record_t", pybind11::arithmetic(), "Type of a net_handle_t, which may not be the type of the record\nThis is to allow a node record to be seen as a chain from the perspective of a handle")
			.value("ROOT_HANDLE", bdsg::SnarlDistanceIndex::ROOT_HANDLE)
			.value("NODE_HANDLE", bdsg::SnarlDistanceIndex::NODE_HANDLE)
			.value("SNARL_HANDLE", bdsg::SnarlDistanceIndex::SNARL_HANDLE)
			.value("CHAIN_HANDLE", bdsg::SnarlDistanceIndex::CHAIN_HANDLE)
			.value("SENTINEL_HANDLE", bdsg::SnarlDistanceIndex::SENTINEL_HANDLE)
			.export_values();


		pybind11::enum_<bdsg::SnarlDistanceIndex::record_t>(cl, "record_t", pybind11::arithmetic(), "A record_t is the type of structure that a record can be.\n The actual distance index is stored as a series of \"records\" for each snarl/node/chain. \n The record type defines what is stored in a record\n\nNODE, SNARL, and CHAIN indicate that they don't store distances.\nSIMPLE_SNARL is a snarl with all children connecting only to the boundary nodes in one direction (ie, a bubble).\nTRIVIAL_SNARL represents consecutive nodes in a chain. \nNODE represents a node that is a trivial chain. A node can only be the child of a snarl.\nOVERSIZED_SNARL only stores distances to the boundaries.\nROOT_SNARL represents a connected component of the root. It has no start or end node so \n   its children technically belong to the root.\nMULTICOMPONENT_CHAIN can represent a chain with snarls that are not start-end connected.\n    The chain is split up into components between these snarls, each node is tagged with\n    which component it belongs to.")
			.value("ROOT", bdsg::SnarlDistanceIndex::ROOT)
			.value("NODE", bdsg::SnarlDistanceIndex::NODE)
			.value("DISTANCED_NODE", bdsg::SnarlDistanceIndex::DISTANCED_NODE)
			.value("TRIVIAL_SNARL", bdsg::SnarlDistanceIndex::TRIVIAL_SNARL)
			.value("DISTANCED_TRIVIAL_SNARL", bdsg::SnarlDistanceIndex::DISTANCED_TRIVIAL_SNARL)
			.value("SIMPLE_SNARL", bdsg::SnarlDistanceIndex::SIMPLE_SNARL)
			.value("DISTANCED_SIMPLE_SNARL", bdsg::SnarlDistanceIndex::DISTANCED_SIMPLE_SNARL)
			.value("SNARL", bdsg::SnarlDistanceIndex::SNARL)
			.value("DISTANCED_SNARL", bdsg::SnarlDistanceIndex::DISTANCED_SNARL)
			.value("OVERSIZED_SNARL", bdsg::SnarlDistanceIndex::OVERSIZED_SNARL)
			.value("ROOT_SNARL", bdsg::SnarlDistanceIndex::ROOT_SNARL)
			.value("DISTANCED_ROOT_SNARL", bdsg::SnarlDistanceIndex::DISTANCED_ROOT_SNARL)
			.value("CHAIN", bdsg::SnarlDistanceIndex::CHAIN)
			.value("DISTANCED_CHAIN", bdsg::SnarlDistanceIndex::DISTANCED_CHAIN)
			.value("MULTICOMPONENT_CHAIN", bdsg::SnarlDistanceIndex::MULTICOMPONENT_CHAIN)
			.value("CHILDREN", bdsg::SnarlDistanceIndex::CHILDREN)
			.export_values();


		pybind11::enum_<bdsg::SnarlDistanceIndex::temp_record_t>(cl, "temp_record_t", pybind11::arithmetic(), "")
			.value("TEMP_CHAIN", bdsg::SnarlDistanceIndex::TEMP_CHAIN)
			.value("TEMP_SNARL", bdsg::SnarlDistanceIndex::TEMP_SNARL)
			.value("TEMP_NODE", bdsg::SnarlDistanceIndex::TEMP_NODE)
			.value("TEMP_ROOT", bdsg::SnarlDistanceIndex::TEMP_ROOT)
			.export_values();

		cl.def("serialize", [](bdsg::SnarlDistanceIndex &o, const std::string & a0) -> void { return o.serialize(a0); }, "", pybind11::arg("filename"));
		cl.def("deserialize", [](bdsg::SnarlDistanceIndex &o, const std::string & a0) -> void { return o.deserialize(a0); }, "", pybind11::arg("filename"));
		cl.def("dissociate", (void (bdsg::SnarlDistanceIndex::*)()) &bdsg::SnarlDistanceIndex::dissociate, "C++: bdsg::SnarlDistanceIndex::dissociate() --> void");
		cl.def("serialize", (void (bdsg::SnarlDistanceIndex::*)(const class std::function<void (const void *, unsigned long)> &) const) &bdsg::SnarlDistanceIndex::serialize, "C++: bdsg::SnarlDistanceIndex::serialize(const class std::function<void (const void *, unsigned long)> &) const --> void", pybind11::arg("iteratee"));
		cl.def("serialize", (void (bdsg::SnarlDistanceIndex::*)(int)) &bdsg::SnarlDistanceIndex::serialize, "C++: bdsg::SnarlDistanceIndex::serialize(int) --> void", pybind11::arg("fd"));
		cl.def("deserialize", (void (bdsg::SnarlDistanceIndex::*)(int)) &bdsg::SnarlDistanceIndex::deserialize, "C++: bdsg::SnarlDistanceIndex::deserialize(int) --> void", pybind11::arg("fd"));
		cl.def("get_magic_number", (unsigned int (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::get_magic_number, "C++: bdsg::SnarlDistanceIndex::get_magic_number() const --> unsigned int");
		cl.def("get_prefix", (std::string (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::get_prefix, "C++: bdsg::SnarlDistanceIndex::get_prefix() const --> std::string");
		cl.def("preload", [](bdsg::SnarlDistanceIndex const &o) -> void { return o.preload(); }, "");
		cl.def("preload", (void (bdsg::SnarlDistanceIndex::*)(bool) const) &bdsg::SnarlDistanceIndex::preload, "Allow for preloading the index for more accurate timing of algorithms\n that use it, if it fits in memory. If blocking is true, waits for the\n index to be paged in. Otherwise, just tells the OS that we will want to\n use it.\n\nC++: bdsg::SnarlDistanceIndex::preload(bool) const --> void", pybind11::arg("blocking"));
		cl.def("maximum_distance", [](bdsg::SnarlDistanceIndex const &o, const long long & a0, const bool & a1, const unsigned long & a2, const long long & a3, const bool & a4, const unsigned long & a5) -> unsigned long { return o.maximum_distance(a0, a1, a2, a3, a4, a5); }, "", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"));
		cl.def("maximum_distance", [](bdsg::SnarlDistanceIndex const &o, const long long & a0, const bool & a1, const unsigned long & a2, const long long & a3, const bool & a4, const unsigned long & a5, bool const & a6) -> unsigned long { return o.maximum_distance(a0, a1, a2, a3, a4, a5, a6); }, "", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"), pybind11::arg("unoriented_distance"));
		cl.def("maximum_distance", (unsigned long (bdsg::SnarlDistanceIndex::*)(const long long, const bool, const unsigned long, const long long, const bool, const unsigned long, bool, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::maximum_distance, "Find an approximation of the maximum distance between two positions. \nThis isn't a true maximum- the only guarantee is that it's greater than or equal to the minimum distance.\n\nC++: bdsg::SnarlDistanceIndex::maximum_distance(const long long, const bool, const unsigned long, const long long, const bool, const unsigned long, bool, const class handlegraph::HandleGraph *) const --> unsigned long", pybind11::arg("id1"), pybind11::arg("rev1"), pybind11::arg("offset1"), pybind11::arg("id2"), pybind11::arg("rev2"), pybind11::arg("offset2"), pybind11::arg("unoriented_distance"), pybind11::arg("graph"));
		cl.def("distance_in_parent", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1, const struct handlegraph::net_handle_t & a2) -> unsigned long { return o.distance_in_parent(a0, a1, a2); }, "", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"));
		cl.def("distance_in_parent", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1, const struct handlegraph::net_handle_t & a2, const class handlegraph::HandleGraph * a3) -> unsigned long { return o.distance_in_parent(a0, a1, a2, a3); }, "", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"), pybind11::arg("graph"));
		cl.def("distance_in_parent", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, unsigned long) const) &bdsg::SnarlDistanceIndex::distance_in_parent, "C++: bdsg::SnarlDistanceIndex::distance_in_parent(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, unsigned long) const --> unsigned long", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"), pybind11::arg("graph"), pybind11::arg("distance_limit"));
		cl.def("distance_in_snarl", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const unsigned long & a1, const bool & a2, const unsigned long & a3, const bool & a4) -> unsigned long { return o.distance_in_snarl(a0, a1, a2, a3, a4); }, "", pybind11::arg("parent"), pybind11::arg("rank1"), pybind11::arg("right_side1"), pybind11::arg("rank2"), pybind11::arg("right_side2"));
		cl.def("distance_in_snarl", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const unsigned long & a1, const bool & a2, const unsigned long & a3, const bool & a4, const class handlegraph::HandleGraph * a5) -> unsigned long { return o.distance_in_snarl(a0, a1, a2, a3, a4, a5); }, "", pybind11::arg("parent"), pybind11::arg("rank1"), pybind11::arg("right_side1"), pybind11::arg("rank2"), pybind11::arg("right_side2"), pybind11::arg("graph"));
		cl.def("distance_in_snarl", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const unsigned long &, const bool &, const unsigned long &, const bool &, const class handlegraph::HandleGraph *, unsigned long) const) &bdsg::SnarlDistanceIndex::distance_in_snarl, "C++: bdsg::SnarlDistanceIndex::distance_in_snarl(const struct handlegraph::net_handle_t &, const unsigned long &, const bool &, const unsigned long &, const bool &, const class handlegraph::HandleGraph *, unsigned long) const --> unsigned long", pybind11::arg("parent"), pybind11::arg("rank1"), pybind11::arg("right_side1"), pybind11::arg("rank2"), pybind11::arg("right_side2"), pybind11::arg("graph"), pybind11::arg("distance_limit"));
		cl.def("max_distance_in_parent", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1, const struct handlegraph::net_handle_t & a2) -> unsigned long { return o.max_distance_in_parent(a0, a1, a2); }, "", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"));
		cl.def("max_distance_in_parent", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, const struct handlegraph::net_handle_t & a1, const struct handlegraph::net_handle_t & a2, const class handlegraph::HandleGraph * a3) -> unsigned long { return o.max_distance_in_parent(a0, a1, a2, a3); }, "", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"), pybind11::arg("graph"));
		cl.def("max_distance_in_parent", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, unsigned long) const) &bdsg::SnarlDistanceIndex::max_distance_in_parent, "Find the maximum distance between two children in the parent. \nThis is the same as distance_in_parent for everything except children of chains\n\nC++: bdsg::SnarlDistanceIndex::max_distance_in_parent(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *, unsigned long) const --> unsigned long", pybind11::arg("parent"), pybind11::arg("child1"), pybind11::arg("child2"), pybind11::arg("graph"), pybind11::arg("distance_limit"));
		cl.def("distance_to_parent_bound", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0, bool const & a1, struct handlegraph::net_handle_t const & a2) -> unsigned long { return o.distance_to_parent_bound(a0, a1, a2); }, "", pybind11::arg("parent"), pybind11::arg("to_start"), pybind11::arg("child"));
		cl.def("distance_to_parent_bound", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, bool, struct handlegraph::net_handle_t, class std::tuple<enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t>) const) &bdsg::SnarlDistanceIndex::distance_to_parent_bound, "Get the distance from the child to the start or end bound of the parent.\nparent_and_child_types are hints to figure out the type of snarl/chain records the parent and child are.\ntuple of parent record type, parent handle type, child record type, child handle type.\nThis is really just used to see if the parent and child are trivial chains, so it might not be exactly what the actual record is.\n\nC++: bdsg::SnarlDistanceIndex::distance_to_parent_bound(const struct handlegraph::net_handle_t &, bool, struct handlegraph::net_handle_t, class std::tuple<enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t>) const --> unsigned long", pybind11::arg("parent"), pybind11::arg("to_start"), pybind11::arg("child"), pybind11::arg("parent_and_child_types"));
		cl.def("into_which_snarl", (class std::tuple<long long, bool, bool> (bdsg::SnarlDistanceIndex::*)(const long long &, const bool &) const) &bdsg::SnarlDistanceIndex::into_which_snarl, "If this node id and orientation is pointing into a snarl, then return the start.\nnode id and orientation pointing into the snarl, and if the snarl is trivial.\nReturns <0, false, false> if it doesn't point into a snarl.\n\nC++: bdsg::SnarlDistanceIndex::into_which_snarl(const long long &, const bool &) const --> class std::tuple<long long, bool, bool>", pybind11::arg("id"), pybind11::arg("reverse"));
		cl.def("is_ordered_in_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_ordered_in_chain, "Return true if child1 comes before child2 in the chain. \n\nC++: bdsg::SnarlDistanceIndex::is_ordered_in_chain(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("child1"), pybind11::arg("child2"));
		cl.def("is_externally_start_end_connected", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::is_externally_start_end_connected, "C++: bdsg::SnarlDistanceIndex::is_externally_start_end_connected(const struct handlegraph::net_handle_t) const --> bool", pybind11::arg("net"));
		cl.def("is_externally_start_start_connected", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::is_externally_start_start_connected, "C++: bdsg::SnarlDistanceIndex::is_externally_start_start_connected(const struct handlegraph::net_handle_t) const --> bool", pybind11::arg("net"));
		cl.def("is_externally_end_end_connected", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::is_externally_end_end_connected, "C++: bdsg::SnarlDistanceIndex::is_externally_end_end_connected(const struct handlegraph::net_handle_t) const --> bool", pybind11::arg("net"));
		cl.def("lowest_common_ancestor", (struct std::pair<struct handlegraph::net_handle_t, bool> (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::lowest_common_ancestor, "For two net handles, get a net handle lowest common ancestor.\nIf the lowest common ancestor is the root, then the two handles may be in\ndifferent connected components. In this case, return false.\n\nC++: bdsg::SnarlDistanceIndex::lowest_common_ancestor(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> struct std::pair<struct handlegraph::net_handle_t, bool>", pybind11::arg("net1"), pybind11::arg("net2"));
		cl.def("node_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::node_length, "Return the length of the net, which must represent a node (or sentinel of a snarl)\n\nC++: bdsg::SnarlDistanceIndex::node_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("minimum_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::minimum_length, "This is also the length of a net, but it can also be a snarl or chain. \nThe length of a chain includes the boundary nodes, a snarl does not.\nA looping chain only includes the start/end node once\n\nC++: bdsg::SnarlDistanceIndex::minimum_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("maximum_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::maximum_length, "C++: bdsg::SnarlDistanceIndex::maximum_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("chain_minimum_length", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::chain_minimum_length, "The length of a chain. If it is a multicomponent chain, then the length of \nthe last component, which is used for calculating distance, instead of inf \n\nC++: bdsg::SnarlDistanceIndex::chain_minimum_length(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("node_id", (long long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::node_id, "What is the node id of the node represented by this net handle.\nnet must be a node or a sentinel\n\nC++: bdsg::SnarlDistanceIndex::node_id(const struct handlegraph::net_handle_t &) const --> long long", pybind11::arg("net"));
		cl.def("has_node", (bool (bdsg::SnarlDistanceIndex::*)(const long long) const) &bdsg::SnarlDistanceIndex::has_node, "Does the graph have this node?\n\nC++: bdsg::SnarlDistanceIndex::has_node(const long long) const --> bool", pybind11::arg("id"));
		cl.def("is_reversed_in_parent", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_reversed_in_parent, "Only really relevant for nodes in chains, is the node\ntraversed backwards relative to the orientation of the chain\n\nC++: bdsg::SnarlDistanceIndex::is_reversed_in_parent(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("get_node_net_handle", [](bdsg::SnarlDistanceIndex const &o, const long long & a0) -> handlegraph::net_handle_t { return o.get_node_net_handle(a0); }, "", pybind11::arg("id"));
		cl.def("get_node_net_handle", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const long long, bool) const) &bdsg::SnarlDistanceIndex::get_node_net_handle, "Get a net handle from a node and, optionally, an orientation\n\nC++: bdsg::SnarlDistanceIndex::get_node_net_handle(const long long, bool) const --> struct handlegraph::net_handle_t", pybind11::arg("id"), pybind11::arg("rev"));
		cl.def("get_max_tree_depth", (unsigned long (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::get_max_tree_depth, "How deep is the snarl tree? The root is 0, top-level chain is 1, etc\nOnly counts chains\n\nC++: bdsg::SnarlDistanceIndex::get_max_tree_depth() const --> unsigned long");
		cl.def("get_depth", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_depth, "What is the depth of this net handle? Nodes and snarls get the depth of their parent.\nThe depth of the root is 0, the depth of its child chains is 1, the depth of the nodes and snarls that are \nchildren of those chains is also 1, and the chains that are children of those snarls have depth 2\n\nC++: bdsg::SnarlDistanceIndex::get_depth(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_connected_component_number", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_connected_component_number, "C++: bdsg::SnarlDistanceIndex::get_connected_component_number(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_handle_from_connected_component", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(unsigned long) const) &bdsg::SnarlDistanceIndex::get_handle_from_connected_component, "Given the connected component number (from get_connected_component_number), get the\nroot-level handle pointing to it.\nIf the connected component is a root-level snarl, then this may return a \"root\" handle,\nbut it will actually point to the snarl\n\nC++: bdsg::SnarlDistanceIndex::get_handle_from_connected_component(unsigned long) const --> struct handlegraph::net_handle_t", pybind11::arg("num"));
		cl.def("has_connectivity", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const) &bdsg::SnarlDistanceIndex::has_connectivity, "Is there a path between the start and end endpoints within the net handle?\n\nC++: bdsg::SnarlDistanceIndex::has_connectivity(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const --> bool", pybind11::arg("net"), pybind11::arg("start"), pybind11::arg("end"));
		cl.def("has_external_connectivity", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const) &bdsg::SnarlDistanceIndex::has_external_connectivity, "Is there a path between the start and end endpoints outside the net handle?\nThis is used for children of the root\n\nC++: bdsg::SnarlDistanceIndex::has_external_connectivity(const struct handlegraph::net_handle_t &, enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) const --> bool", pybind11::arg("net"), pybind11::arg("start"), pybind11::arg("end"));
		cl.def("get_prefix_sum_value", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_prefix_sum_value, "Get the prefix sum value for a node in a chain.\nFails if the parent of net is not a chain\n\nC++: bdsg::SnarlDistanceIndex::get_prefix_sum_value(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_max_prefix_sum_value", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_max_prefix_sum_value, "Get the maximum prefix sum value for a node in a chain.\nFails if the parent of net is not a chain\n\nC++: bdsg::SnarlDistanceIndex::get_max_prefix_sum_value(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_forward_loop_value", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_forward_loop_value, "Get the forward loop value for a node in a chain.\nFails if the parent of net is not a chain\n\nC++: bdsg::SnarlDistanceIndex::get_forward_loop_value(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_reverse_loop_value", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_reverse_loop_value, "Get the reverse value for a node in a chain.\nFails if the parent of net is not a chain\n\nC++: bdsg::SnarlDistanceIndex::get_reverse_loop_value(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("get_chain_component", [](bdsg::SnarlDistanceIndex const &o, const struct handlegraph::net_handle_t & a0) -> unsigned long { return o.get_chain_component(a0); }, "", pybind11::arg("net"));
		cl.def("get_chain_component", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, bool) const) &bdsg::SnarlDistanceIndex::get_chain_component, "C++: bdsg::SnarlDistanceIndex::get_chain_component(const struct handlegraph::net_handle_t &, bool) const --> unsigned long", pybind11::arg("net"), pybind11::arg("get_end"));
		cl.def("get_root", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::get_root, "Get a net handle referring to a tip-to-tip traversal of the contents of the root snarl.\n\nC++: bdsg::SnarlDistanceIndex::get_root() const --> struct handlegraph::net_handle_t");
		cl.def("is_root", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_root, "Return true if the given handle refers to (a traversal of) the root\nsnarl, and false otherwise.\n\nC++: bdsg::SnarlDistanceIndex::is_root(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_root_snarl", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_root_snarl, "Return true if the given handle refers to (a traversal of) a snarl of the root,\nwhich is considered to be the root but actually refers to a subset of the children \nof the root that are connected\n\nC++: bdsg::SnarlDistanceIndex::is_root_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_snarl", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_snarl, "Returns true if the given net handle refers to (a traversal of) a snarl.\n\nC++: bdsg::SnarlDistanceIndex::is_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_dag", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_dag, "Return true if the given snarl is a DAG and false otherwise\nReturns true if the given net_handle_t is not a snarl\n\nC++: bdsg::SnarlDistanceIndex::is_dag(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("snarl"));
		cl.def("is_simple_snarl", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_simple_snarl, "Returns true if the given net handle refers to (a traversal of) a simple snarl\nA simple snarl is a bubble where each child node can only reach the boundary nodes,\nand each side of a node reaches a different boundary node\nThere may also be an edge connecting the two boundary nodes but no additional \nedges are allowed\n\nC++: bdsg::SnarlDistanceIndex::is_simple_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_regular_snarl", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_regular_snarl, "Returns true if the given net handle refers to (a traversal of) a regular snarl\nA regular snarl is the same as a simple snarl, except that the children may be\nnested chains, rather than being restricted to nodes \n\nC++: bdsg::SnarlDistanceIndex::is_regular_snarl(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_chain, "Returns true if the given net handle refers to (a traversal of) a chain.\n\nC++: bdsg::SnarlDistanceIndex::is_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_multicomponent_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_multicomponent_chain, "Returns true if the given net handle refers to (a traversal of) a chain that is not start-end connected\n\nC++: bdsg::SnarlDistanceIndex::is_multicomponent_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_looping_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_looping_chain, "Returns true if the given net handle refers to (a traversal of) a chain that loops (a chain where the first and last node are the same).\n\nC++: bdsg::SnarlDistanceIndex::is_looping_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_trivial_chain", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_trivial_chain, "Returns true if the given net handle refers to (a traversal of) a trivial chain that represents a single node.\n\nC++: bdsg::SnarlDistanceIndex::is_trivial_chain(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_node", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_node, "Returns true if the given net handle refers to (a traversal of) a single node, and thus has a corresponding handle_t.\n\nC++: bdsg::SnarlDistanceIndex::is_node(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("is_sentinel", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::is_sentinel, "Return true if the given net handle is a snarl bound sentinel (in either\ninward or outward orientation), and false otherwise.\n\nC++: bdsg::SnarlDistanceIndex::is_sentinel(const struct handlegraph::net_handle_t &) const --> bool", pybind11::arg("net"));
		cl.def("get_net", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::get_net, "Turn a handle to an oriented node into a net handle for a start-to-end or end-to-start traversal of the node, as appropriate.\n\nC++: bdsg::SnarlDistanceIndex::get_net(const struct handlegraph::handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::net_handle_t", pybind11::arg("handle"), pybind11::arg("graph"));
		cl.def("get_handle", (struct handlegraph::handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const) &bdsg::SnarlDistanceIndex::get_handle, "For a net handle to a traversal of a single node, get the handle for that node in the orientation it is traversed.\nMay not be called for other net handles.\n\nC++: bdsg::SnarlDistanceIndex::get_handle(const struct handlegraph::net_handle_t &, const class handlegraph::HandleGraph *) const --> struct handlegraph::handle_t", pybind11::arg("net"), pybind11::arg("graph"));
		cl.def("get_parent", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_parent, "Get the parent snarl of a chain, or the parent chain of a snarl or node.\nIf the child is start-to-end or end-to-start, and the parent is a chain,\nthe chain comes out facing the same way, accounting for the relative\norientation of the child snarl or node in the chain. Otherwise,\neverything is produced as start-to-end, even if that is not actually a\nrealizable traversal of a snarl or chain. May not be called on the root\nsnarl.\n\nAlso works on snarl boundary sentinels.\n\nC++: bdsg::SnarlDistanceIndex::get_parent(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("child"));
		cl.def("get_bound", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, bool, bool) const) &bdsg::SnarlDistanceIndex::get_bound, "Get the bounding handle for the snarl or chain referenced by the given\nnet handle, getting the start or end facing in or out as appropriate.\n\nFor snarls, returns the bounding sentinel net handles. For chains,\nreturns net handles for traversals of the bounding nodes of the chain.\nIf the chain is a looping chain, then the start and end of the chain\nare the same, so the connectivity of the bound indicates which we're\nlooking at; the connectivity will be start-start if it is going \nbackwards in the node, and end-end if it is going forwards.\n\nIgnores traversal type.\n\nMay not be called on traversals of individual nodes.\n\nC++: bdsg::SnarlDistanceIndex::get_bound(const struct handlegraph::net_handle_t &, bool, bool) const --> struct handlegraph::net_handle_t", pybind11::arg("snarl"), pybind11::arg("get_end"), pybind11::arg("face_in"));
		cl.def("get_node_from_sentinel", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_node_from_sentinel, "Given the sentinel of a snarl, return a handle to the node representing it\n\nC++: bdsg::SnarlDistanceIndex::get_node_from_sentinel(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("sentinel"));
		cl.def("flip", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::flip, "Return a net handle to the same snarl/chain/node in the opposite orientation.\nNo effect on tip-to-tip, start-to-start, or end-to-end net handles. Flips all the others.\n\nC++: bdsg::SnarlDistanceIndex::flip(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("canonical", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::canonical, "Get a canonical traversal handle from any net handle. All handles to the\nsame net graph element have the same canonical traversal. That canonical\ntraversal must be realizable, and might not always be start-to-end or\neven consistently be the same kind of traversal for different snarls,\nchains, or nodes. Mostly useful to normalize for equality comparisons.\n\nAny root snarl will become just the root\nAnything without connectivity will get START_END\n\nC++: bdsg::SnarlDistanceIndex::canonical(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("start_end_traversal_of", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::start_end_traversal_of, "Makes a start-end traversal of the net.\nFaster than canonical because it doesn't check the index for anything \n\nC++: bdsg::SnarlDistanceIndex::start_end_traversal_of(const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("net"));
		cl.def("starts_at", (enum handlegraph::SnarlDecomposition::endpoint_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::starts_at, "Return the kind of location at which the given traversal starts.\n\nC++: bdsg::SnarlDistanceIndex::starts_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("ends_at", (enum handlegraph::SnarlDecomposition::endpoint_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::ends_at, "Return the kind of location at which the given traversal ends.\n\nC++: bdsg::SnarlDistanceIndex::ends_at(const struct handlegraph::net_handle_t &) const --> enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("traversal"));
		cl.def("get_rank_in_parent", (unsigned long (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_rank_in_parent, "For a child of a snarl, the rank is used to calculate the distance\n\nC++: bdsg::SnarlDistanceIndex::get_rank_in_parent(const struct handlegraph::net_handle_t &) const --> unsigned long", pybind11::arg("net"));
		cl.def("connected_component_count", (unsigned long (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::connected_component_count, "How many connected components are in this graph?\nThis returns the number of topological connected components, not necessarily the \nnumber of nodes in the top-level snarl \n\nC++: bdsg::SnarlDistanceIndex::connected_component_count() const --> unsigned long");
		cl.def("get_snarl_child_from_rank", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const unsigned long &) const) &bdsg::SnarlDistanceIndex::get_snarl_child_from_rank, "Get the child of a snarl from its rank. This shouldn't be exposed to the public interface but I need it\nPlease don't use it\nFor 0 or 1, returns the sentinel facing in. Otherwise return the child as a chain going START_END\n\nC++: bdsg::SnarlDistanceIndex::get_snarl_child_from_rank(const struct handlegraph::net_handle_t &, const unsigned long &) const --> struct handlegraph::net_handle_t", pybind11::arg("snarl"), pybind11::arg("rank"));
		cl.def("get_parent_traversal", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::get_parent_traversal, "Get a net handle for traversals of a snarl or chain that contains\nthe given oriented bounding node traversals or sentinels. Given two\nsentinels for a snarl, produces a net handle to a start-to-end,\nend-to-end, end-to-start, or start-to-start traversal of that snarl.\nGiven handles to traversals of the bounding nodes of a chain, similarly\nproduces a net handle to a traversal of the chain.\n\nFor a chain, either or both handles can also be a snarl containing tips,\nfor a tip-to-start, tip-to-end, start-to-tip, end-to-tip, or tip-to-tip\ntraversal. Similarly, for a snarl, either or both handles can be a chain\nin the snarl that contains internal tips, or that has no edges on the\nappropriate end.\n\nMay only be called if a path actually exists between the given start\nand end.\n\nC++: bdsg::SnarlDistanceIndex::get_parent_traversal(const struct handlegraph::net_handle_t &, const struct handlegraph::net_handle_t &) const --> struct handlegraph::net_handle_t", pybind11::arg("traversal_start"), pybind11::arg("traversal_end"));
		cl.def_static("has_distances", (const bool (*)(enum bdsg::SnarlDistanceIndex::record_t)) &bdsg::SnarlDistanceIndex::has_distances, "C++: bdsg::SnarlDistanceIndex::has_distances(enum bdsg::SnarlDistanceIndex::record_t) --> const bool", pybind11::arg("type"));
		cl.def_static("get_record_handle_type", (const enum bdsg::SnarlDistanceIndex::net_handle_record_t (*)(enum bdsg::SnarlDistanceIndex::record_t)) &bdsg::SnarlDistanceIndex::get_record_handle_type, "Given the type of the record, return the handle type. Some record types can represent multiple things,\nfor example a simple snarl record is used to represent a snarl, and the nodes/trivial chains in it.\nThis will return whatever is higher on the snarl tree. A simple snarl will be considered a snarl,\na root snarl will be considered a root, etc\n\nC++: bdsg::SnarlDistanceIndex::get_record_handle_type(enum bdsg::SnarlDistanceIndex::record_t) --> const enum bdsg::SnarlDistanceIndex::net_handle_record_t", pybind11::arg("type"));
		cl.def_static("get_record_offset", (const unsigned long (*)(const struct handlegraph::net_handle_t &)) &bdsg::SnarlDistanceIndex::get_record_offset, "The offset into records that this handle points to\n\nC++: bdsg::SnarlDistanceIndex::get_record_offset(const struct handlegraph::net_handle_t &) --> const unsigned long", pybind11::arg("net_handle"));
		cl.def_static("get_node_record_offset", (const unsigned long (*)(const struct handlegraph::net_handle_t &)) &bdsg::SnarlDistanceIndex::get_node_record_offset, "The offset of a node in a trivial snarl (0 if it isn't a node in a trivial snarl)\n\nC++: bdsg::SnarlDistanceIndex::get_node_record_offset(const struct handlegraph::net_handle_t &) --> const unsigned long", pybind11::arg("net_handle"));
		cl.def_static("get_connectivity", (const enum bdsg::SnarlDistanceIndex::connectivity_t (*)(const struct handlegraph::net_handle_t &)) &bdsg::SnarlDistanceIndex::get_connectivity, "C++: bdsg::SnarlDistanceIndex::get_connectivity(const struct handlegraph::net_handle_t &) --> const enum bdsg::SnarlDistanceIndex::connectivity_t", pybind11::arg("net_handle"));
		cl.def_static("get_handle_type", (const enum bdsg::SnarlDistanceIndex::net_handle_record_t (*)(const struct handlegraph::net_handle_t &)) &bdsg::SnarlDistanceIndex::get_handle_type, "C++: bdsg::SnarlDistanceIndex::get_handle_type(const struct handlegraph::net_handle_t &) --> const enum bdsg::SnarlDistanceIndex::net_handle_record_t", pybind11::arg("net_handle"));
		cl.def_static("get_net_handle_from_values", [](unsigned long const & a0, enum bdsg::SnarlDistanceIndex::connectivity_t const & a1, enum bdsg::SnarlDistanceIndex::net_handle_record_t const & a2) -> const handlegraph::net_handle_t { return bdsg::SnarlDistanceIndex::get_net_handle_from_values(a0, a1, a2); }, "", pybind11::arg("pointer"), pybind11::arg("connectivity"), pybind11::arg("type"));
		cl.def_static("get_net_handle_from_values", (const struct handlegraph::net_handle_t (*)(unsigned long, enum bdsg::SnarlDistanceIndex::connectivity_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, unsigned long)) &bdsg::SnarlDistanceIndex::get_net_handle_from_values, "C++: bdsg::SnarlDistanceIndex::get_net_handle_from_values(unsigned long, enum bdsg::SnarlDistanceIndex::connectivity_t, enum bdsg::SnarlDistanceIndex::net_handle_record_t, unsigned long) --> const struct handlegraph::net_handle_t", pybind11::arg("pointer"), pybind11::arg("connectivity"), pybind11::arg("type"), pybind11::arg("node_offset"));
		cl.def("get_net_handle", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(unsigned long, enum bdsg::SnarlDistanceIndex::connectivity_t) const) &bdsg::SnarlDistanceIndex::get_net_handle, "C++: bdsg::SnarlDistanceIndex::get_net_handle(unsigned long, enum bdsg::SnarlDistanceIndex::connectivity_t) const --> struct handlegraph::net_handle_t", pybind11::arg("pointer"), pybind11::arg("connectivity"));
		cl.def("get_net_handle", (struct handlegraph::net_handle_t (bdsg::SnarlDistanceIndex::*)(unsigned long) const) &bdsg::SnarlDistanceIndex::get_net_handle, "C++: bdsg::SnarlDistanceIndex::get_net_handle(unsigned long) const --> struct handlegraph::net_handle_t", pybind11::arg("pointer"));
		cl.def_static("get_node_pointer_offset", (const unsigned long (*)(const long long &, const long long &, unsigned long)) &bdsg::SnarlDistanceIndex::get_node_pointer_offset, "Get the offset into snarl_tree_records for the pointer to a node record.\n\nC++: bdsg::SnarlDistanceIndex::get_node_pointer_offset(const long long &, const long long &, unsigned long) --> const unsigned long", pybind11::arg("id"), pybind11::arg("min_node_id"), pybind11::arg("component_count"));
		cl.def_static("endpoints_to_connectivity", (const enum bdsg::SnarlDistanceIndex::connectivity_t (*)(enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t)) &bdsg::SnarlDistanceIndex::endpoints_to_connectivity, "C++: bdsg::SnarlDistanceIndex::endpoints_to_connectivity(enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t) --> const enum bdsg::SnarlDistanceIndex::connectivity_t", pybind11::arg("start"), pybind11::arg("end"));
		cl.def_static("get_start_endpoint", (const enum handlegraph::SnarlDecomposition::endpoint_t (*)(enum bdsg::SnarlDistanceIndex::connectivity_t)) &bdsg::SnarlDistanceIndex::get_start_endpoint, "C++: bdsg::SnarlDistanceIndex::get_start_endpoint(enum bdsg::SnarlDistanceIndex::connectivity_t) --> const enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("connectivity"));
		cl.def_static("get_start_endpoint", (const enum handlegraph::SnarlDecomposition::endpoint_t (*)(struct handlegraph::net_handle_t)) &bdsg::SnarlDistanceIndex::get_start_endpoint, "C++: bdsg::SnarlDistanceIndex::get_start_endpoint(struct handlegraph::net_handle_t) --> const enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("net"));
		cl.def_static("get_end_endpoint", (const enum handlegraph::SnarlDecomposition::endpoint_t (*)(enum bdsg::SnarlDistanceIndex::connectivity_t)) &bdsg::SnarlDistanceIndex::get_end_endpoint, "C++: bdsg::SnarlDistanceIndex::get_end_endpoint(enum bdsg::SnarlDistanceIndex::connectivity_t) --> const enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("connectivity"));
		cl.def_static("get_end_endpoint", (const enum handlegraph::SnarlDecomposition::endpoint_t (*)(const struct handlegraph::net_handle_t &)) &bdsg::SnarlDistanceIndex::get_end_endpoint, "C++: bdsg::SnarlDistanceIndex::get_end_endpoint(const struct handlegraph::net_handle_t &) --> const enum handlegraph::SnarlDecomposition::endpoint_t", pybind11::arg("net"));
		cl.def_static("connectivity_to_endpoints", (const struct std::pair<enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t> (*)(const enum bdsg::SnarlDistanceIndex::connectivity_t &)) &bdsg::SnarlDistanceIndex::connectivity_to_endpoints, "C++: bdsg::SnarlDistanceIndex::connectivity_to_endpoints(const enum bdsg::SnarlDistanceIndex::connectivity_t &) --> const struct std::pair<enum handlegraph::SnarlDecomposition::endpoint_t, enum handlegraph::SnarlDecomposition::endpoint_t>", pybind11::arg("connectivity"));
		cl.def("set_snarl_size_limit", (void (bdsg::SnarlDistanceIndex::*)(unsigned long)) &bdsg::SnarlDistanceIndex::set_snarl_size_limit, "C++: bdsg::SnarlDistanceIndex::set_snarl_size_limit(unsigned long) --> void", pybind11::arg("size"));
		cl.def("set_only_top_level_chain_distances", (void (bdsg::SnarlDistanceIndex::*)(bool)) &bdsg::SnarlDistanceIndex::set_only_top_level_chain_distances, "C++: bdsg::SnarlDistanceIndex::set_only_top_level_chain_distances(bool) --> void", pybind11::arg("only_chain"));
		cl.def("net_handle_as_string", (std::string (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &) const) &bdsg::SnarlDistanceIndex::net_handle_as_string, "C++: bdsg::SnarlDistanceIndex::net_handle_as_string(const struct handlegraph::net_handle_t &) const --> std::string", pybind11::arg("net"));
		cl.def("traverse_decomposition", (bool (bdsg::SnarlDistanceIndex::*)(const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &bdsg::SnarlDistanceIndex::traverse_decomposition, "C++: bdsg::SnarlDistanceIndex::traverse_decomposition(const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("snarl_iteratee"), pybind11::arg("chain_iteratee"), pybind11::arg("node_iteratee"));
		cl.def("traverse_decomposition_helper", (bool (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const) &bdsg::SnarlDistanceIndex::traverse_decomposition_helper, "C++: bdsg::SnarlDistanceIndex::traverse_decomposition_helper(const struct handlegraph::net_handle_t &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &, const class std::function<bool (const struct handlegraph::net_handle_t &)> &) const --> bool", pybind11::arg("net"), pybind11::arg("snarl_iteratee"), pybind11::arg("chain_iteratee"), pybind11::arg("node_iteratee"));
		cl.def("print_self", (void (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::print_self, "C++: bdsg::SnarlDistanceIndex::print_self() const --> void");
		cl.def("print_descendants_of", (void (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::print_descendants_of, "C++: bdsg::SnarlDistanceIndex::print_descendants_of(const struct handlegraph::net_handle_t) const --> void", pybind11::arg("net"));
		cl.def("print_snarl_stats", (void (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::print_snarl_stats, "C++: bdsg::SnarlDistanceIndex::print_snarl_stats() const --> void");
		cl.def("write_snarls_to_json", (void (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::write_snarls_to_json, "C++: bdsg::SnarlDistanceIndex::write_snarls_to_json() const --> void");
		cl.def("validate_index", (void (bdsg::SnarlDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::validate_index, "C++: bdsg::SnarlDistanceIndex::validate_index() const --> void");
		cl.def("validate_descendants_of", (void (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::validate_descendants_of, "C++: bdsg::SnarlDistanceIndex::validate_descendants_of(const struct handlegraph::net_handle_t) const --> void", pybind11::arg("net"));
		cl.def("validate_ancestors_of", (void (bdsg::SnarlDistanceIndex::*)(const struct handlegraph::net_handle_t) const) &bdsg::SnarlDistanceIndex::validate_ancestors_of, "C++: bdsg::SnarlDistanceIndex::validate_ancestors_of(const struct handlegraph::net_handle_t) const --> void", pybind11::arg("net"));
		cl.def("get_usage", (class std::tuple<unsigned long, unsigned long, unsigned long> (bdsg::SnarlDistanceIndex::*)()) &bdsg::SnarlDistanceIndex::get_usage, "C++: bdsg::SnarlDistanceIndex::get_usage() --> class std::tuple<unsigned long, unsigned long, unsigned long>");
		cl.def_static("sum", (unsigned long (*)(const unsigned long &, const unsigned long &)) &bdsg::SnarlDistanceIndex::sum, "Add integers, returning max() if any of them are max()\n\nC++: bdsg::SnarlDistanceIndex::sum(const unsigned long &, const unsigned long &) --> unsigned long", pybind11::arg("val1"), pybind11::arg("val2"));
		cl.def_static("minus", (unsigned long (*)(unsigned long, unsigned long)) &bdsg::SnarlDistanceIndex::minus, "C++: bdsg::SnarlDistanceIndex::minus(unsigned long, unsigned long) --> unsigned long", pybind11::arg("x"), pybind11::arg("y"));
		cl.def_static("maximum", (unsigned long (*)(unsigned long, unsigned long)) &bdsg::SnarlDistanceIndex::maximum, "C++: bdsg::SnarlDistanceIndex::maximum(unsigned long, unsigned long) --> unsigned long", pybind11::arg("x"), pybind11::arg("y"));
		cl.def_static("bit_width", (unsigned long (*)(unsigned long)) &bdsg::SnarlDistanceIndex::bit_width, "C++: bdsg::SnarlDistanceIndex::bit_width(unsigned long) --> unsigned long", pybind11::arg("value"));
		cl.def("time_accesses", (void (bdsg::SnarlDistanceIndex::*)()) &bdsg::SnarlDistanceIndex::time_accesses, "C++: bdsg::SnarlDistanceIndex::time_accesses() --> void");

		{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex file:bdsg/snarl_distance_index.hpp line:1524
			auto & enclosing_class = cl;
			pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex>> cl(enclosing_class, "TemporaryDistanceIndex", "");
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
			cl.def_readwrite("use_oversized_snarls", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::use_oversized_snarls);
			cl.def("structure_start_end_as_string", (std::string (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::*)(struct std::pair<enum bdsg::SnarlDistanceIndex::temp_record_t, unsigned long>) const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::structure_start_end_as_string, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::structure_start_end_as_string(struct std::pair<enum bdsg::SnarlDistanceIndex::temp_record_t, unsigned long>) const --> std::string", pybind11::arg("index"));
			cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::*)() const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::get_max_record_length() const --> unsigned long");
			cl.def("assign", (class bdsg::SnarlDistanceIndex::TemporaryDistanceIndex & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::*)(const class bdsg::SnarlDistanceIndex::TemporaryDistanceIndex &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::operator=(const class bdsg::SnarlDistanceIndex::TemporaryDistanceIndex &) --> class bdsg::SnarlDistanceIndex::TemporaryDistanceIndex &", pybind11::return_value_policy::automatic, pybind11::arg(""));

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord file:bdsg/snarl_distance_index.hpp line:1544
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord>> cl(enclosing_class, "TemporaryRecord", "");
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord(o); } ) );
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord(); } ) );
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord file:bdsg/snarl_distance_index.hpp line:1546
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporaryChainRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord(o); } ) );
				cl.def_readwrite("start_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::start_node_id);
				cl.def_readwrite("end_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_id);
				cl.def_readwrite("end_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_length);
				cl.def_readwrite("tree_depth", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::tree_depth);
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::parent);
				cl.def_readwrite("min_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::min_length);
				cl.def_readwrite("max_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::max_length);
				cl.def_readwrite("distance_left_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::distance_left_start);
				cl.def_readwrite("distance_right_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::distance_right_start);
				cl.def_readwrite("distance_left_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::distance_left_end);
				cl.def_readwrite("distance_right_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::distance_right_end);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::rank_in_parent);
				cl.def_readwrite("root_snarl_index", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::root_snarl_index);
				cl.def_readwrite("start_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::start_node_rev);
				cl.def_readwrite("end_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::end_node_rev);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::reversed_in_parent);
				cl.def_readwrite("is_trivial", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::is_trivial);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::is_tip);
				cl.def_readwrite("loopable", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::loopable);
				cl.def_readwrite("children", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::children);
				cl.def_readwrite("prefix_sum", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::prefix_sum);
				cl.def_readwrite("max_prefix_sum", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::max_prefix_sum);
				cl.def_readwrite("forward_loops", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::forward_loops);
				cl.def_readwrite("backward_loops", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::backward_loops);
				cl.def_readwrite("chain_components", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::chain_components);
				cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::*)(bool) const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::get_max_record_length(bool) const --> unsigned long", pybind11::arg("include_distances"));
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryChainRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord file:bdsg/snarl_distance_index.hpp line:1588
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporarySnarlRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord(o); } ) );
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::parent);
				cl.def_readwrite("start_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_id);
				cl.def_readwrite("start_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_length);
				cl.def_readwrite("end_node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_id);
				cl.def_readwrite("end_node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_length);
				cl.def_readwrite("node_count", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::node_count);
				cl.def_readwrite("min_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::min_length);
				cl.def_readwrite("max_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::max_length);
				cl.def_readwrite("max_distance", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::max_distance);
				cl.def_readwrite("tree_depth", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::tree_depth);
				cl.def_readwrite("distance_start_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::distance_start_start);
				cl.def_readwrite("distance_end_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::distance_end_end);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::rank_in_parent);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::reversed_in_parent);
				cl.def_readwrite("start_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::start_node_rev);
				cl.def_readwrite("end_node_rev", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::end_node_rev);
				cl.def_readwrite("is_trivial", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_trivial);
				cl.def_readwrite("is_simple", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_simple);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_tip);
				cl.def_readwrite("is_root_snarl", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::is_root_snarl);
				cl.def_readwrite("include_distances", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::include_distances);
				cl.def_readwrite("children", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::children);
				cl.def_readwrite("tippy_child_ranks", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::tippy_child_ranks);
				cl.def_readwrite("distances", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::distances);
				cl.def("get_max_record_length", (unsigned long (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::*)() const) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::get_max_record_length() const --> unsigned long");
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporarySnarlRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

			{ // bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord file:bdsg/snarl_distance_index.hpp line:1621
				auto & enclosing_class = cl;
				pybind11::class_<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord, std::shared_ptr<bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord>, bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryRecord> cl(enclosing_class, "TemporaryNodeRecord", "");
				cl.def( pybind11::init( [](){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord(); } ) );
				cl.def( pybind11::init( [](bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord const &o){ return new bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord(o); } ) );
				cl.def_readwrite("node_id", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::node_id);
				cl.def_readwrite("parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::parent);
				cl.def_readwrite("node_length", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::node_length);
				cl.def_readwrite("rank_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::rank_in_parent);
				cl.def_readwrite("root_snarl_index", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::root_snarl_index);
				cl.def_readwrite("distance_left_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::distance_left_start);
				cl.def_readwrite("distance_right_start", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::distance_right_start);
				cl.def_readwrite("distance_left_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::distance_left_end);
				cl.def_readwrite("distance_right_end", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::distance_right_end);
				cl.def_readwrite("reversed_in_parent", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::reversed_in_parent);
				cl.def_readwrite("is_tip", &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::is_tip);
				cl.def_static("get_max_record_length", (const unsigned long (*)()) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::get_max_record_length, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::get_max_record_length() --> const unsigned long");
				cl.def("assign", (struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord & (bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::*)(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord &)) &bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::operator=, "C++: bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord::operator=(const struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord &) --> struct bdsg::SnarlDistanceIndex::TemporaryDistanceIndex::TemporaryNodeRecord &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			}

		}

	}
}
