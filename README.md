# zoiperExample
Quick example: How Integrate zoiper SDK on a swift project.

Requisite:
* install git-lfs.github.com

Inside ZoiperExample:
On AppDelegate update user information:

ZoiperVoip.sharedInstance().registerSIP(
withUser: "<user>", pass: "<pass>", server: "<server>", proxy: "<proxy>")
