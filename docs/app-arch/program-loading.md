# Application Storage and Loading

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
    "entry_points":[
        {
            "name": "function1",
            "allow_anonymous": true
        }
    ]
}
```