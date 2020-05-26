module.exports = {extends: ['@commitlint/config-conventional']}

module.exports = {
  extends: ['@commitlint/config-conventional'],
  rules: {
    'type-enum': [2, 'always', [
      "feat", "fix", "perf", "refactor", "style", "build", "revert", "docs", "test",
      "chore", "milestone", "release", "developing", "patch"
    ]]
  }
};
