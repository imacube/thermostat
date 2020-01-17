Update the commit history.

```shell script
git filter-branch --env-filter '

OLD_EMAIL="imacube@gmail.com"
CORRECT_NAME="Ryan"
CORRECT_EMAIL="imacube@gmail.com"

if [ "$GIT_COMMITTER_EMAIL" = "$OLD_EMAIL" ]
then
    export GIT_COMMITTER_NAME="$CORRECT_NAME"
    export GIT_COMMITTER_EMAIL="$CORRECT_EMAIL"
fi
if [ "$GIT_AUTHOR_EMAIL" = "$OLD_EMAIL" ]
then
    export GIT_AUTHOR_NAME="$CORRECT_NAME"
    export GIT_AUTHOR_EMAIL="$CORRECT_EMAIL"
fi
' --tag-name-filter cat -- --branches --tags
```

Push the changes.

```shell script
git push --force --tags origin 'refs/heads/*'
```

Source: https://help.github.com/en/github/using-git/changing-author-info
