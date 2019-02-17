from bottle import route, post, run, request
from datetime import datetime
def main():
    run(host='0.0.0.0', port=8080, debug=True)

@route('/')
def root():
    """ GET /
    Returns a "Hello, World!" message
    """
    return "Hello, World!\n"

@post('/log')
def log():
    name = request.forms.get('name')
    temp = request.forms.get('temp')
    print("{} Update from {}: {}°C".format(datetime.now().replace(microsecond=0).isoformat(), name, temp))
    return

if __name__ == "__main__":
    main()