#ifndef BOOST_JSON_REPLAY_HANDLER_HPP
#define BOOST_JSON_REPLAY_HANDLER_HPP


#include <boost/variant.hpp>
#include <boost/none.hpp>
#include <vector>

namespace boost {
namespace json {

struct replay_handler
{
    struct document_begin_op { boost::none_t arg; };
    struct document_end_op  { boost::none_t arg; };
    struct object_begin_op  { boost::none_t arg; };
    struct object_end_op  { std::size_t arg; };
    struct array_begin_op  { boost::none_t arg; };
    struct array_end_op  { std::size_t arg; };
    struct key_part_op  { std::string arg; };
    struct key_op  { std::string arg; };
    struct string_part_op  { std::string arg; };
    struct string_op  { std::string arg; };
    struct int64_op { std::int64_t arg; };
    struct uint64_op { std::uint64_t arg; };
    struct double_op { double arg; };
    struct bool_op { bool arg; };
    struct null_op { boost::none_t arg; };

    using element = boost::variant<
        document_begin_op, document_end_op,
        object_begin_op, object_end_op,
        array_begin_op, array_end_op,
        key_part_op, key_op,
        string_part_op, string_op,
        int64_op, uint64_op, double_op, bool_op, null_op>;

    std::vector<element> tape_;

    template<class Handler> static void invoke_op(Handler&& h, document_begin_op const&, error_code& ec )
    {
        h.on_document_begin(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, document_end_op const&, error_code& ec )
    {
        h.on_document_end(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, object_begin_op const&, error_code& ec )
    {
        h.on_object_begin(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, object_end_op const& op, error_code& ec )
    {
        h.on_object_end(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, array_begin_op const&, error_code& ec )
    {
        h.on_array_begin(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, array_end_op const& op, error_code& ec )
    {
        h.on_document_begin(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, key_part_op const& op, error_code& ec )
    {
        h.on_key_part(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, key_op const& op, error_code& ec )
    {
        h.on_key(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, string_part_op const& op, error_code& ec )
    {
        h.on_string_part(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, string_op const&, error_code& ec )
    {
        h.on_string(ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, int64_op const& op, error_code& ec )
    {
        h.on_int64(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, uint64_op const& op, error_code& ec )
    {
        h.on_uint64(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, double_op const& op, error_code& ec )
    {
        h.on_double(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, bool_op const& op, error_code& ec )
    {
        h.on_bool(op.arg, ec);
    }
    template<class Handler> static void invoke_op(Handler&& h, null_op const&, error_code& ec )
    {
        h.on_null(ec);
    }

    template<class Handler>
    struct invoker
    {
        Handler& handler_;

        template<class OpType>
        error_code operator()(OpType const& op) const
        {
            error_code ec;
            invoke_op(handler_, op, ec);
            return ec;
        }
    };

    static std::string to_string(string_view arg)
    {
        return std::string(arg.begin(), arg.end());
    }

    bool on_document_begin(error_code&)  { tape_.emplace_back(document_begin_op{ boost::none }); return true; }
    bool on_document_end(error_code&)  { tape_.emplace_back(document_end_op{ boost::none }); return true; }
    bool on_object_begin(error_code&)  { tape_.emplace_back(object_begin_op{ boost::none }); return true; }
    bool on_object_end(std::size_t arg, error_code&)  { tape_.emplace_back(object_end_op{ arg }); return true; }
    bool on_array_begin(error_code&)  { tape_.emplace_back(array_begin_op{ boost::none }); return true; }
    bool on_array_end(std::size_t arg, error_code&)  { tape_.emplace_back(array_end_op{ arg }); return true; }
    bool on_key_part(string_view arg, error_code&)  { tape_.emplace_back(key_part_op{to_string(arg)}); return true; }
    bool on_key( string_view arg, error_code&)  { tape_.emplace_back(key_op{to_string(arg)}); return true; }
    bool on_string_part(string_view arg, error_code&) { tape_.emplace_back(string_part_op{to_string(arg)}); return true; }
    bool on_string(string_view arg, error_code&)  { tape_.emplace_back(string_op{to_string(arg)}); return true; }
    bool on_int64(std::int64_t arg, error_code&)  { tape_.emplace_back(int64_op{arg}); return true; }
    bool on_uint64(std::uint64_t arg, error_code&)  { tape_.emplace_back(uint64_op{arg}); return true; }
    bool on_double(double arg, error_code&)  { tape_.emplace_back(double_op{arg}); return true; }
    bool on_bool(bool arg, error_code&)  { tape_.emplace_back(bool_op{arg}); return true; }
    bool on_null(error_code&)  { tape_.emplace_back(null_op{ boost::none }); return true; }

};

}
}

#endif
