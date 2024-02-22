export DEBIAN_FRONTEND="noninteractive"
export LANG="en_US.UTF-8"
export LANGUAGE="en_US.UTF-8"
export LC_ALL="C.UTF-8"
export DISPLAY=":0.0"
export DISPLAY_WIDTH="1024"
export DISPLAY_HEIGHT="768"

exec supervisord -c /workspaces/concept-os/graphics-env/supervisord.conf &