# SQL-BOF
A library of beacon object files to interact with remote SQL servers and data. This collection is templated off the TrustedSec [CS-Situational-Awareness-BOF](https://github.com/trustedsec/CS-Situational-Awareness-BOF) collection and models the functionality of the [SQLRecon](https://github.com/skahwah/SQLRecon) project.
Some beacon object file for BRC4. Based on [SQL-BOF](https://github.com/Tw1sm/SQL-BOF).

## Usage
- coffexec /~/MSSQL-BOF-BRC4/SQL/xpcmd/xpcmd.x64.o "hostname && whoami"

## Available commands
|Commands|Usage|Notes|
|--------|-----|-----|
|coffexec /~/MSSQL-BOF-BRC4/SQL/1434udp/1434udp.x64.o 127.0.0.1|[server IP] |Enumerate SQL Server connection info |
|coffexec /~/MSSQL-BOF-BRC4/SQL/enableXpcmd/enableXpcmd.x64.o 1|[new Value for xp_cmdshell] |Change value of xp_cmdshell |
|coffexec /~/MSSQL-BOF-BRC4/SQL/info/info.x64.o|Gather information about the SQL server |
|coffexec /~/MSSQL-BOF-BRC4/SQL/query/query.x64.o "SELECT name,value FROM sys.configurations WHERE name = 'xp_cmdshell'"|[query]|Execute a custom SQL query |
|coffexec /~/MSSQL-BOF-BRC4/SQL/whoami/whoami.x64.o |Gather logged in user, mapped user and roles |
|coffexec /~/MSSQL-BOF-BRC4/SQL/xpcmd/xpcmd.x64.o "hostname && whoami" |[command]|Execute a system command via xp_cmdshell |

## References
- [SQL-BOF](https://github.com/Tw1sm/SQL-BOF)
- [SQLRecon](https://github.com/skahwah/SQLRecon) by [@sanjivkawa](https://twitter.com/sanjivkawa)
- [PySQLRecon](https://github.com/Tw1sm/PySQLRecon)
- [CS-Situational-Awareness-BOF](https://github.com/trustedsec/CS-Situational-Awareness-BOF)


## Disclaimer
This repo is provided exclusively for educational and informational purposes. The contents of this repository are intended to be used solely as a learning resource. The authors of this repository expressly disclaim any responsibility for any misuse or unintended application of the tools, code, or information provided within this repository.
Users are solely responsible for ensuring that their use of the repository complies with applicable laws and regulations. The authors of this repository do not provide any warranties or guarantees regarding the accuracy, completeness, or suitability of the contents for any particular purpose.
If you do not agree with these terms, you are advised not to use or access this repository.
