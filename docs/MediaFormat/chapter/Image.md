# 图片

# JPEG

## 介绍

JPEG 首字母缩略词代表联合图像专家组，创建 JPEG 交换格式 (JIF) 标准的小组委员会。于 1992 年由国际标准化组织 (ISO) 首次发布。JPEG 是 24 位静止光栅图像，RGB 颜色模型的每个通道中有 8 位。这没有为 Alpha 通道留下空间，这意味着虽然 JPEG 可以支持超过 1600 万种颜色，但它们不能支持透明度。JPEG 压缩基于一种称为离散余弦变换 (DCT) 的有损图像压缩技术，图像占用的存储空间减少了 50-75%（与 BMP 等旧格式相比），但图像质量几乎没有损失
- `JPEG/JFIF` : 存储和传输照片图像最流行的格式
- `JPEG/Exif` : 数码相机和其他图像捕获设备的格式

## JPG

早期版本的 Windows（特别是 MS-DOS 8.3 和 FAT-16 文件系统）在文件扩展名长度方面有最多 3 个字母的限制。JPEG 必须缩短为 JPG，以免超过限制。Mac 和 Linux 计算机从来没有这样的东西，所以用户会继续将图像保存为 JPEG。因此，**JPG 与 JPEG 其实是同一个东西** 。

# Exif

## 介绍

[Exif（Exchangeable Image File）](https://www.media.mit.edu/pia/Research/deepview/exif.html)：是存储在 `JPEG` 格式照片头部的一段信息，会储存相机型号、镜头、曝光、图片尺寸等信息，相机和手机拍摄的照片都会携带这些信息，。PS照片采用低质量保存会丢失这些信息，但在10-12等级的时候不会丢失，美图秀秀保存质量为100%才不会丢失。软件在处理的时候也会将自己的信息写入Exif，因此可以通过Exif判断是否为原图。

## 旋转信息

 在 `Exif` 中会储存一个特殊的信息 `Orientation`，用于表示相机拍摄的照片相对于地面的位置。图片实际存储关系与相机界面保持一致，但是通过软件查看时，就会首先读取图片的旋转信息，然后根据旋转信息展示图片。

![orentation](../../image/media/orientation.png)

`Orientation` 取值 `8`，0行 `左`，0列 `下` 表示：
- 旋转后图片的上方是原图的左方
- 旋转后图片的左方是原图的下方


| 取值 | 0行（未旋转上） | 0列（未旋转左） | 旋转效果           |
| :--- | :-------------- | :-------------- | :----------------- |
| 1    | 上              | 左              | 0°                 |
| 2    | 上              | 右              | 水平翻转           |
| 3    | 下              | 右              | 180°               |
| 4    | 下              | 左              | 垂直翻转           |
| 5    | 左              | 上              | 顺时针90°+水平翻转 |
| 6    | 右              | 上              | 顺时针90°          |
| 7    | 右              | 下              | 顺时针90°+垂直翻转 |
| 8    | 左              | 下              | 逆时针90°          |



![value](../../image/media/orientationValue.png)


## Exiftool

```term
triangle@LEARN:~$ exiftool -Orientation=6  test.jpg // 给一张jpg 图片添加旋转信息
triangle@LEARN:~$ exiftool -Orientation=  test.jpg // 移除旋转信息
triangle@LEARN:~$ exiftool -Orientation  test.jpg // 查看旋转信息

```
