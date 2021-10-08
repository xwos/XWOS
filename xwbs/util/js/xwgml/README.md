# 功能

每次```git commit```时检查commit message是否符合规范。

# 部署

## 安装node.js

到官网下载LTS的版本node，当前测试可用的版本为14.18.0：

<https://nodejs.org/zh-cn/download/>

安装路径可以选择```/usr/local/```或```~/.local/```，大部分Linux发行版
的```PATH```环境变量已经包含这两个路径。

安装完成后可使用下面命令测试一下版本号。

``` shell
node -v
```

## 修改npm的仓库源

打开文件```~/.npmrc```，增加下列配置，使用国内的镜像。

``` shell
registry = https://registry.npm.taobao.org
```

## 安装commitlint与husky

``` shell
npm install --save-dev
```

# 测试

``` shell
git commit --allow-empty -m "badtype: badsubject"
```

因commit message不符合规范，提交会被husky中止。

``` shell
⧗   input: badtype: badsubject
✖   type must be one of [feat, fix, perf, refactor, style, build, revert, docs, test, chore, milestone, release, revision, alpha, beta, rc] [type-enum]

✖   found 1 problems, 0 warnings
ⓘ   Get help: https://github.com/conventional-changelog/commitlint/#what-is-commitlint

husky - commit-msg hook exited with code 1 (error)
```
