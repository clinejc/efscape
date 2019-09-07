Running the efscape server
==========================
To run the efscape server, first start the IceGrid service:

```
icegridnode --Ice.Config=config.grid
```

This command will not automatially run in the background and return to the
command line prompt.

In a separate window:

```
icegridadmin --Ice.Config=config.grid -e "application add application.xml"
efpyclient
```

Messages will be displayed in the IceGrid service window.