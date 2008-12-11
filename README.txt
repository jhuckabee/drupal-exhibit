// $Id: README.txt,v 1.5 2008/07/24 13:16:13 arto Exp $

Exhibit for Drupal
==================
This is an-alpha version of the Exhibit module currently being developed
for Drupal 6.x under the auspices of the project at:

  <http://drupal.org/project/exhibit>

For more information about Exhibit, see the project's home page at SIMILE:

  <http://simile.mit.edu/exhibit/>


BUG REPORTS
-----------
Post bug reports and feature requests to the issue tracking system at:

  <http://drupal.org/node/add/project_issue/exhibit>


USAGE
-----
See INSTALL.txt for installation and basic configuration of the Exhibit
module.  Once you have the module installed and configured and some feeds
to work with, you can create an Exhibit node at node/add/exhibit.

The Exhibit node contains two different definition fields, one for the main
body of the Exhibit, and the other for the Exhibit facet.  The main facet
definition is where you will include the various views, and lenses for your
Exhibit.  If you don't know what these are, see Exhibit links above.

The facet definition allows you to separate your facets from the main body of
your Exhibit.  These facets get rendered in a block that you can configure at
admin/build/block.  Make sure that if you are going to be putting your facets
inside the facet definition, that the Exhibit facets block is enabled for your
Exhibit path.

Beneath the facet definition is a simple facet generator.  The facet generator
looks at the feeds that you have selected and allows you to create facets based
on the fields within those feeds.  The facet generator will limit the various
facet types that are available based on type of field you have selected.  The
types are defined in the "properties" section of the Exhibit feed.  Once you
have selected a facet type, various options for that facet will be available
for you.  These options are automatically set to their default state.  Once
you are satisfied with the facet options, click the "Generate Facet HTML"
button and facet HTML will be generated and appended to the Facet definition
field.

Once your Exhibit definitions are complete, save the node, and you should have
a fully functioning Exhibit rendered on the front end.


CREDITS
-------
Developed and maintained by Arto Bendiken <http://bendiken.net/>
Co-maintained by Josh Huckabee <http://joshhuckabee.com/>
Co-maintained by Djun Kim <http://www.puregin.org/>
Sponsored by MakaluMedia Group <http://www.makalumedia.com/>
Sponsored by M.C. Dean, Inc. <http://www.mcdean.com/>
Sponsored by SPAWAR <http://www.spawar.navy.mil/>
