# Terminal UI

Application to host a terminal interface.

```json
{
    "id": "os.terminal-ui",
    "name": "Terminal UI",
    "allow_anonymous_connect": false,
    "entry_points":[
        {
            "name": "notify"
        },
        {
            "name": "get_buffer"
        },
        {
            "name": "transfer_control" 
        }
    ],
    "notify_of_apps": [
        "os.text-mode-driver"
    ]
}
```