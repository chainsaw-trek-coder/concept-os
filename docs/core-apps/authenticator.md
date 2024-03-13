# Authenticator

Responsible for authenticating users. Generates one time use tokens to start applications.

- Any app can call authenticator.
- Tokens are one time use.

```json
{
    "name": "Authenticator",
    "allow_anonymous_connect": true,
    "entry_points":[
        {
            "name": "login_by_terminal"
        }
    ]
}
```