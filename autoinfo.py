# -*- coding: cp936 -*-
#!/bin/python
import os
import _winreg
from subprocess import Popen, PIPE

def GetSystemPath():
	key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE,r"SYSTEM\CurrentControlSet\Control\Session Manager\Environment")
	value, type = _winreg.QueryValueEx(key, "Path")
	return value

if __name__ == '__main__':
	if os.sys.version_info.major != 2 or os.sys.version_info.minor != 7:
		print("Sorry, Only support python 2.7.x!");
		exit(1)
	# ����VC�Ļ���������û��Git�����ԣ�������ϵͳ�Ļ�������������ᵼ��git����ʧ��
	os.environ["PATH"] = os.getenv('PATH') + GetSystemPath()

	# ����Ƿ���δ�ύ�ĸĶ�
	f = Popen("git status -s", stdout=PIPE)
	f.wait()
	PrefixCommit = ""
	for line in f.stdout.readlines():
		if line.find(".rc") > 0:
			continue
		if line.find(" M ") == 0 or line.find(" D ") == 0:
			if PrefixCommit != "*":
				print(" \r\n");
				print(" \r\n");
				print("Warn: Please commit the change to git first:\r\n");
			PrefixCommit = "*"
		if PrefixCommit == "*":
			print("    %s" % line);
	if PrefixCommit == "*":
		print(" \r\n");
		print(" \r\n");
		print(" \r\n");

	# ��ȡ��ǰ��CommitID
	f = Popen("git show -s", stdout=PIPE)
	f.wait()
	for line in f.stdout.readlines():
		if line.find("commit") < 0:
			continue
		CommitID = line.lstrip("commit").strip()
		break
	
	# ���û�з����ı䣬�������ļ�
	if os.path.exists('gitcommit.log'):
		f = open('gitcommit.log', 'r+')
		OldCommitID = f.read()
		f.close()
		if OldCommitID == "\"%s%s\"\n" % (PrefixCommit, CommitID):
			print("Same CommitID %s%s" % (PrefixCommit, CommitID));
			exit()

	print("New CommitID %s%s \r\n  \r\n" % (PrefixCommit, CommitID));

	# ����gitinfo.log��gitcommit.log
	f = open('gitinfo.log', 'w+')
	f.write("// commitid: %s%s\n" % (PrefixCommit, CommitID))
	if PrefixCommit == "*":
		f.write("{0x%s, 0x%s, 0x%s, 0x%s, 0x0};\n" % (CommitID[0:8], CommitID[8:16], CommitID[16:24], CommitID[24:32]))
	else:
		f.write("{0x%s, 0x%s, 0x%s, 0x%s, 0x%s};\n" % (CommitID[0:8], CommitID[8:16], CommitID[16:24], CommitID[24:32], CommitID[32:]))
	f.close()
	
	f = open('gitcommit.log', 'w+')
	f.write("\"%s%s\"\n" % (PrefixCommit, CommitID))
	f.close()

	# �����ļ�������޸�ʱ�䣬�Ա��ڱ�����������ļ�
	if len(os.sys.argv) > 1 and os.path.exists(os.sys.argv[1]):
		f = open(os.sys.argv[1], 'rb')
		t = f.read()
		f.close()
		f = open(os.sys.argv[1], 'wb')
		f.write(t)
		f.close()