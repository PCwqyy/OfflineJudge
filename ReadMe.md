# Offline Judge

试图伪装成 OJ 的对拍器。  
前往 [github](https://github.com/PCwqyy/OfflineJudge) Release 可下载最新版本。

### 注意事项

- 必须在虚拟终端下运行，如果默认为 conhost 会自动切换到 Windows Terminal.
- 用 `Tab` 、 `Enter` 、 `Esc` 和方向键操作。
- 所有你的程序应放在 `./Code/` 文件夹下，并按下表命名：

|程序|名称|
|-:|:-|
|待测程序|`mine.exe`|
|标准程序|`std.exe`|
|数据生成程序|`data.exe`|

- 每次最多拍 50000 组，最少拍 2 组。
- 由于使用了 `scanf()` ，如果你在任何地方输入了奇怪的字符 ( `/`, `\` 之类 ) 会导致卡流死循环，重启程序即可。

### v1.1 Release
- 程序内编译
- 更详细的评测信息
- 优化了性能

### v1.0 Release

- 现已支持 MLE
- 程序内查看记录