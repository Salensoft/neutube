from bottle import route, run, get, post, request, static_file, abort, hook, response
import json
import subprocess
import sys
import socket
import jsonschema
import httplib
import socket
import os

sys.path.append('..')
sys.path.append('../..')
sys.path.append('../../module')
sys.path.append('../../flyem')

socket.setdefaulttimeout(1000)

import skeletonize as skl
import neutu_server as ns

@get('/home')
def home():
    return ns.home('skeletonize')

@get('/skeletonize')
def skeletonize():
    return '''
        <form action="/skeletonize" method="post">
            Body ID: <input name="bodyId" type="text"/>
            <input value="Submit" type="submit"/>
        </form>
    '''

@post('/skeletonize')
def do_skeletonize():
    print request.content_type
    bodyArray = [];
    dvidServer = ns.getDefaultDvidServer()
    uuid = ns.getDefaultUuid()
    if request.content_type == 'application/x-www-form-urlencoded':
        bodyIdStr = request.forms.get('bodyId')
        bodyArray = [int(bodyId) for bodyId in bodyIdStr.split()]
    elif request.content_type == 'application/json':
        print request.json
        jsonObj = request.json
        try:
            jsonschema.validate(jsonObj, json.loads(getSchema('skeletonize', 'post')))
        except jsonschema.exceptions.ValidationError as inst:
            print 'Invalid json input'
            print inst
            return '<p>Skeletonization for ' + str(bodyArray) + ' failed.</p>'
        uuid = jsonObj['uuid']
        if jsonObj.has_key('dvid-server'):
            dvidServer = jsonObj['dvid-server']
        bodyArray = jsonObj['bodies']
    
    output = {}
    config = {'dvid-server': dvidServer, 'uuid': uuid}

    print '********'
    print config
    
    output["swc-list"] = []
    for bodyId in bodyArray:
        conn = httplib.HTTPConnection(dvidServer)
        bodyLink = '/api/node/' + uuid + '/skeletons/' + str(bodyId) + '.swc'
        print '************', bodyLink
        conn.request("GET", bodyLink)

        r1 = conn.getresponse()
        if not r1.status == 200:
            try:
                skl.Skeletonize(bodyId, 'dvid', config)
            except Exception as inst:
                return '<p>' + str(inst) + '</p>'
            
        swc = {"id": bodyId, "url": dvidServer + bodyLink}
        output["swc-list"].append(swc)
    
    return json.dumps(output, sort_keys = False)

@hook('after_request')
def enable_cors(fn=None):
    def _enable_cors(*args, **kwargs):
        print 'enable cors'
        response.headers['Access-Control-Allow-Origin'] = '*'
        response.headers['Access-Control-Expose-Headers'] = 'Content-Type'
        response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, OPTIONS'
        response.headers['Access-Control-Allow-Headers'] = 'Origin, Accept, Content-Type, X-Requested-With, X-CSRF-Token'
        if request.method != 'OPTIONS':
            return fn(*args, **kwargs)
    return _enable_cors

@route('/interface/interface.raml', method=['GET', 'OPTIONS'])
@enable_cors
def retrieveRaml():
    print 'retrieve raml'
    fileResponse = static_file('interface.raml', root='.', mimetype='application/raml+yaml')
    fileResponse.headers['Access-Control-Allow-Origin'] = '*'

    return fileResponse

def get_json_post():
    return ns.get_json_post(request)
        
@post('/json')
def parseJson():
    data = get_json_post()
    return '<p>' + data['head'] + '</p>'

print 'Starting the server ...'
run(host=socket.gethostname(), port=8080, debug=True)

# print getSchema('skeletonize', 'post')
# try:
#     jsonschema.validate({"bodies": [1, 2, 3]}, json.loads(getSchema('skeletonize', 'post')))
# except jsonschema.exceptions.ValidationError as inst:
#     print 'Invalid json input'
#     print inst
