#include "erl_nif.h"

#define A_OK(env)            enif_make_atom(env, "ok")
#define assert_badarg(S, Env) if (! S) { return enif_make_badarg(env); }

static ErlNifResourceType* array_handle = NULL;

static void array_handle_cleanup(ErlNifEnv* env, void* arg) {}

static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    ErlNifResourceFlags flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
    array_handle = enif_open_resource_type(env, "native_array_nif", "array_handle",
                                           &array_handle_cleanup, flags, 0);
    // 用于存储指针的数组, 最多1000个array
    *priv = enif_alloc(1000 * sizeof(void*));
    return 0;
}

static void unload(ErlNifEnv* env, void* priv)
{
    enif_free(priv);
}

static ERL_NIF_TERM new_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // 取参数
    int refindex;
    assert_badarg(enif_get_int(env, argv[0], &refindex), env);
    // 取参数length
    unsigned long length;
    assert_badarg(enif_get_ulong(env, argv[1], &length), env);
    // 分配内存
    // unsigned char* ref = enif_alloc_resource(array_handle, length);
    unsigned char* ref = enif_alloc(length);
    // 保存指针
    *((unsigned char**)enif_priv_data(env) + refindex) = ref;
    return A_OK(env);
}

static ERL_NIF_TERM get_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // 取参数ref
    int refindex;
    assert_badarg(enif_get_int(env, argv[0], &refindex), env);
    unsigned char* ref = *((unsigned char**)enif_priv_data(env) + refindex);
    assert_badarg(ref, env);
    // 取参数offset
    unsigned long offset;
    assert_badarg(enif_get_ulong(env, argv[1], &offset), env);
    return enif_make_int(env, (int)(*(ref + offset - 1)));
}

static ERL_NIF_TERM put_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // 取参数ref
    int refindex;
    assert_badarg(enif_get_int(env, argv[0], &refindex), env);
    unsigned char* ref = *((unsigned char**)enif_priv_data(env) + refindex);
    // 取参数offset
    unsigned long offset;
    assert_badarg(enif_get_ulong(env, argv[1], &offset), env);
    // 取参数newval
    unsigned int newval;
    assert_badarg(enif_get_uint(env, argv[2], &newval), env);
    // 赋值
    *(ref + offset - 1) = (unsigned char)newval;
    return A_OK(env);
}

static ERL_NIF_TERM delete_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // 取参数ref
    int refindex;
    assert_badarg(enif_get_int(env, argv[0], &refindex), env);
    unsigned char* ref = *((unsigned char**)enif_priv_data(env) + refindex);
    //enif_release_resource(ref); 
    enif_free(ref); 
    return A_OK(env);
}

static ErlNifFunc nif_funcs[] = {
    {"new", 2, new_nif},
    {"get", 2, get_nif},
    {"put", 3, put_nif},
    {"delete", 1, delete_nif},
};

ERL_NIF_INIT(native_array, nif_funcs, &load, NULL, NULL, &unload)

