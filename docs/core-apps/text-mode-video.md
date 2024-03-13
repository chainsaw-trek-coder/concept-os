# Text Mode Video Driver

Driver that provides a basic interface with the text mode video buffer.

- Any app can connect?
- Only one app can be displayed at a time.
- Apps must surrender access.
- If an app disconnects, falls back to anonymous app.

Draft Manifest:

```json
{
    "id": "os.text-mode-driver",
    "name": "Text Mode Driver",
    "allow_anonymous_connect": true,
    "entry_points":[
        {
            "name": "get_buffer"
        },
        {
            "name": "get_transfer_token" 
        },
        {
            "name": "transfer_control"
        }
    ]
}
```