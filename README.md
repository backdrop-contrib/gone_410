410 Gone Or 404
===============
Whenever you unpublish a node, all anonymous visitors will receive a "403 Access Denied" error when they try to open the URL.
This confuses people and clutters the log.

This module allows you to add the URLs of unpublished pages to a special list.

It returns a "410 Gone" response code instead 403, which helps search engines remove these URLs from their searches.

If you temporarily unpublish a node, you may want to set it to return a "404 Not Found",
which confuses people less, and then remove it from the list when you publish it again.



Installation
------------
Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

Configuration and usage
-----------------------
On *Administration > Configuration >
URL Handling > 410 Gone Or 404 Not Found* menu (admin/config/urls/gone-410)
is available an administration page where you can add or remove such URLs.

Possible issues
---------------
This module uses hook_boot, so you may sometimes see log messages like this:
> "Warning: Cannot modify header information - headers already sent by (output started at /core/includes/bootstrap.inc..."

Here is the explanation.

When Backdrop served a cached page, the 'X-Backdrop-Cache: HIT' and 'cache-control' headers were sent with the obsolete entries before they were actually generated for the request.

To avoid such messages and incorrect module actions (in such cases can not get in time to return right response) you have two options:

- you can disable prefetching for cached pages: go to 'admin/config/development/performance' and within the 'Caching' fieldset uncheck the 'Use background fetch for cached pages' checkbox, then press the 'Save configuration' button;
- add the option '$settings['page_cache_invoke_hooks'] = TRUE;' to your 'settings.php' file.

Disabling prefetching for cached pages (first option) is sufficient to avoid such collisions in most cases.

License
-------
This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

Current Maintainer
------------------
Vladimir (https://github.com/findlabnet/)

Issues
------
For bug reports, feature or support requests, please use the module issue queue at https://github.com/backdrop-contrib/gone_410/issues
