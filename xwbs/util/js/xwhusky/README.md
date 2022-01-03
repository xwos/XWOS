# 简介

Husky是一个git hook的框架，可用来检查提交信息、运行测试、检查代码等等。

# 安装

``` shell
npm i
npm run prepare
```

# 功能

## commitlint

### 说明

执行```git commit```时检查```commit message```是否符合规范。
规范配置：```commitlint.config.js```

### 测试

``` shell
git commit --allow-empty -m "badtype: badsubject"
```

因commit message不符合规范，提交会被husky中止。

``` shell
⧗   input: badtype: badsubject
✖   type must be one of [feat, fix, perf, refactor, style, revert, docs, test, chore, milestone, release, revision, alpha, beta, rc] [type-enum]

✖   found 1 problems, 0 warnings
ⓘ   Get help: https://github.com/conventional-changelog/commitlint/#what-is-commitlint

husky - commit-msg hook exited with code 1 (error)
```
