module.exports = {extends: ['@commitlint/config-conventional']}

module.exports = {
  extends: ['@commitlint/config-conventional'],
  rules: {
    'type-enum': [2, 'always', [
      "feat", "fix", "perf", "refactor", "style", "revert", "docs", "test",
      "chore", "milestone", "release", "revision", "alpha", "beta", "rc"
    ]]
  }
};
