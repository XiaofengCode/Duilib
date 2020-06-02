本项目是由本人基于原始Duilib维护而来，和当前他人使用的Duilib可能不兼容

一些较大的改动（目前整理出来这些，后续可能还会补充）：

1、支持StringTable，但是不支持动态切换
	StringTable的格式为：
	<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
	<String default="en_us">
		<en_us>
			<IDS_XXX>123</IDS_XXX>
			<IDS_XXX2>#IDS_XXX#456</IDS_XXX2>
			<!-- IDS_XXX2 对应的字符串为123456，两个#之间可以引用其他ID-->
		</en_us>
	</String>
	或
	<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
	<String>
		<IDS_XXX>XXX</IDS_XXX>
	</String>
	
	皮肤文件里可以使用Window的StringTable属性指定StringTable的文件名，默认为String.xml
	引用方法：
	<Label text="#IDS_XXX"/>
	注意：所有字符串ID必须以“IDS_”开头，这是为了方便区分普通字符串还是字符串ID
	
2、基于default扩展了style属性，支持嵌套Style
3、控件名和属性不区分大小写
4、属性限定符不区分先后顺序如hotbkcolor与bkhotcolor等同，即color的限定符顺序不区分
5、将XML改为pugixml，语法支持更标准，同时也更严格，比如之前属性之间手误没有空格也能解析通过，但是现在不行了
6、所有内置控件都基于Container，即所有控件内都可以嵌套其他控件
7、WindowImplBase新增一些常用函数GetDlgItem，SetDlgItemText等，GetDlgItem会从当前虚拟窗口内查找，没有虚拟窗口则从根查找
8、虚拟窗口可以嵌套，同时Option的Group会自动区分虚拟窗口，即不同的虚拟窗口引用同一个xml文件不再担心Option的Group重复的问题
9、Font改用字符串名称，不再使用声明顺序
10、预处理命令可以在任何使用之前声明，不需要必须紧跟在Window后声明
11、改善了TreeView的可用性
