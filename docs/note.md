## Jottings

### MSVC /MT & /MD 编译选项异常

检查 /MT 编译模式：仅 `PhysXExtensions_static_64.lib` 有 `/DEFAULTLIB:OLDNAMES`和`/DEFAULTLIB:LIBCMT` 的标志，其他所有库都没有，难怪之前CMake未能成功设置MT编译选项时，报错也仅发生在该lib库中

```powershell
dumpbin /directives PhysXExtensions_static_64.lib | findstr "DEFAULTLIB"
```

