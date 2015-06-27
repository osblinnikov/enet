from helper import *
#           Environment
Import( 'env' )

def add_dependencies(env):
  '''[[[cog
  import cogging as c
  c.tpl(cog,templateFile,c.a(prefix=configFile))
  ]]]'''

  '''[[[end]]] (checksum: 68b329da9893e34099c7d8ad5cb9c940)'''
#  AddPthreads(env)
  AddNetwork(env)

c = {}
# c['runFiles'] = ['main.c']
# c['sourceFiles'] = ['types.c']
c['PROG_NAME'] = 'enet_osblinnikov_github_com'
c['testFiles'] = ['enetTests.c']
c['inclDeps'] = add_dependencies
c['defines'] = []

if "vc" not in env["COMPILER"]:
    c['defines'] += ["HAS_SOCKLEN_T=1"]
    c['defines'] += ["HAS_MSGHDR_FLAGS=1"]

def defineIfFuncExists(funcName, define):
    conf = Configure(env['scons'])
    if conf.CheckFunc(funcName):
        c['defines'] += [define]
    env['scons'] = conf.Finish()
defineIfFuncExists('fcntl', 'HAS_FCNT=1')
defineIfFuncExists('poll', 'HAS_POLL=1')
defineIfFuncExists('getaddrinfo', 'HAS_GETADDRINFO=1')
defineIfFuncExists('getnameinfo', 'HAS_GETNAMEINFO=1')
defineIfFuncExists('gethostbyname_r', 'HAS_GETHOSTBYNAME_R=1')
defineIfFuncExists('gethostbyaddr_r', 'HAS_GETHOSTBYADDR_R=1')
defineIfFuncExists('inet_pton', 'HAS_INET_PTON=1')
defineIfFuncExists('inet_ntop', 'HAS_INET_NTOP=1')

DefaultLibraryConfig(env, c)
