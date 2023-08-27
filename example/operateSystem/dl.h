#define REC_SZ 32
#define DL_MAGIC "\x01\x14\x05\x14"

#ifdef __ASSEMBLER__

  /* 文件头 */
  #define DL_HEAD     __hdr: \
                      /* magic */    .ascii DL_MAGIC; \
                      /* file_sz */  .4byte (__end - __hdr); \
                      /* code_off */ .4byte (__code - __hdr)

  /* 代码段 */
  #define DL_CODE     .fill REC_SZ - 1, 1, 0; \
                      .align REC_SZ, 0; \
                      __code:
  #define DL_END      __end:

  /**
   * 标签汇编代码生成
   * .align REC_SZ, 0: 字节对齐方式
   * sym: 标签
   *     .8byte (off); 标签偏移量
   *     .ascii name  类型 + 标签名
  */
  #define RECORD(sym, off, name) \
    .align REC_SZ, 0; \ 
    sym \
        .8byte (off); \
        .ascii name


  /**
   * 初始化导入符号表
   *  offset: 0 
   *  type: ? ，表示当前不知道
   *  name: sym
  */
  #define IMPORT(sym) RECORD(sym:,           0, "?" #sym "\0")

  /**
   * 定义导出符号表
   *  offset: sym - __hdr，相对于头部标签的偏移量
   *  type: # ，需要导出
   *  name: sym
  */
  #define EXPORT(sym) RECORD(    , sym - __hdr, "#" #sym "\0")

  /**
   * 加载动态库
   *  offset: 0
   *  type: + ，动态库
   *  name: lib
  */
  #define LOAD(lib)   RECORD(    ,           0, "+" lib  "\0")

  /**
   * 调用符号
  */
  #define DSYM(sym)   *sym(%rip)
#else
  #include <stdint.h>

  /* 动态库的文件结构 */

  // 文件头
  struct dl_hdr {
    char magic[4]; // 头部标记位
    uint32_t file_sz; // 文件大小
    uint32_t code_off; // 代码段相对与文件头的偏移量
  };

  // 标签
  struct symbol {
    int64_t offset; // 符号所在的真实位置
    char type ; // 类型 
    char name[REC_SZ - sizeof(int64_t) - 1]; // 符号名
  };
#endif
