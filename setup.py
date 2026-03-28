from setuptools import setup
import subprocess
import re
from packaging.tags import sys_tags

tag = next(sys_tags())

def get_git_url():
    url = 'https://gitcode.com/Ascend/mstx'
    try:
        # 获取远程 origin 的 URL
        result = subprocess.run(
            ["git", "config", "--get", "remote.origin.url"],
            capture_output=True,
            text=True,
            check=False
        )
        if result.returncode == 0:
            url = result.stdout.strip()
            # 处理 SSH 地址（如 git@github.com:user/repo.git） → 转为 HTTPS
            if url.startswith("ssh://"):
                url = url.replace("ssh://", "https://").replace(":", "/", 1)
            if url.startswith("git@"):
                url = url.replace("git@", "https://").replace(":", "/", 1)
            re.sub(r':\d+', '', url)
            return url
        else:
            print("无法获取 Git URL，使用默认值")
            return url
    except Exception as e:
        print("无法自动获取 Git URL，使用默认值")
        return url

setup(
    name = 'mstx',
    version = '26.0.0',
    author =' mstx',
    author_email = 'mstx',
    description = 'mstx',
    long_description = open('README.md', encoding='utf-8').read(),
    long_description_content_type = 'text/markdown',
    url = get_git_url(),
    options={
        'bdist_wheel':{
            'plat_name': tag.platform}},
    packages = ['lib64', 'include'],
    include_package_data = True,
    license= 'MIT',
    classifiers = [
        'Programming Language :: Python :: 3',
        'Operating System :: Linux',
    ],
    python_requires = '>=3.7'
)
