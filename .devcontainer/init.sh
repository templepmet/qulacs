echo "UID=$(id -u $USER)" > .devcontainer/.env
echo "GID=$(id -g $USER)" >> .devcontainer/.env
echo "USERNAME=$USER" >> .devcontainer/.env
