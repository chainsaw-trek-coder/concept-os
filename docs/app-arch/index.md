# Applications

Binaries:

- in the ELF format.
- Have multiple entry points.

Application Packages:

- Contain binaries
- Contain a manifest.

Application Manifest

- Lists entry points.
- Determines which entry points allow anonymous access.

```json
{
    "name": "Example app",
    "allow_anonymous_connect": true,
    "sandbox_connections": false,
    "entry_points":[
        {
            "name": "function1"
        }
    ]
}
```

Every application has the following functions:

- connect - Loads application if not loaded and associates it with another app.

Applications can optionally implement:

- notify - Receive notfications from the OS. For example when applications or drivers load.
- suspend - Suspend state of connected application within this application.
- resume - resume state of connected application to this application.
- disconnect - Notifies application that another application has disconnected from it.
