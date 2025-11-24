<h2 align="center">Remote Rcon enable for ReHLDS</h2>
<p align="center">Enable ReHLDS to send rcon command to other servers.</p>

<p align="center">
    <a href="https://github.com/SmileYzn/rcon/issues"><img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/rcon?style=flat-square"></a>
    <a href="https://github.com/SmileYzn/rcon/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/rcon/build.yml?branch=main&label=Build&style=flat-square"></a>
</p>

### What this plugin do
This plugin can for example control the remote HLTV server without player need to use their console.
So, server admins can automate an HLTV system or replicate the server commands to other servers.

### Usage
rr_send "command" - Send a command to a remote server.

The result of command will be shown in HLDS console.

### Plugin variables

| Variable      | Description      |
|---------------|------------------|
| rr_address    | Remote Server Address   |
| rr_password   | Remote Server Rcon Password    |
| rr_port       | Remote Server Rcon Port        |
