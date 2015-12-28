-module(native_array).
-export([new/2, get/2, put/3, delete/1]).
-on_load(init/0).

init() ->
    ok = erlang:load_nif("./native_array_nif", 0).

new(_Idx, _Length) ->
    exit(nif_library_not_loaded).

get(_Idx, _Offset) ->
    exit(nif_library_not_loaded).

put(_Idx, _Offset, _NewVal) ->
    exit(nif_library_not_loaded).

delete(_Idx) ->
    exit(nif_library_not_loaded).

