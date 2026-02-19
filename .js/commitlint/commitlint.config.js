module.exports = {extends: ['@commitlint/config-conventional']}

module.exports = {
    extends: ['@commitlint/config-conventional'],
    rules: {
        'subject-case': [
            2,
            'never',
            ['start-case', 'pascal-case'],
        ],
        'type-enum': [
            2,
            'always',
            ["feat", "fix", "workaround", "perf", "refactor", "style", "revert", "docs", "test", "ci",
             "chore", "milestone", "release", "revision", "alpha", "beta", "rc"
            ]
        ]
    }
};
