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

def defineIfFuncExists(env, c, funcName, define):
    conf = Configure(env['scons'])
    if conf.CheckFunc(funcName):
        c['defines'] += [define]
    env['scons'] = conf.Finish()

c = {}
c['PROG_NAME'] = 'enet_osblinnikov_github_com'
# c['sourceFiles'] = ['types.c']
#c['testFiles'] = ['typesTest.c']
# c['runFiles'] = ['main.c']
c['inclDeps'] = add_dependencies
c['defines'] = []

defineIfFuncExists(env, c, 'fcntl', 'HAS_FCNT=1')
defineIfFuncExists(env, c, 'poll', 'HAS_POLL=1')
defineIfFuncExists(env, c, 'getaddrinfo', 'HAS_GETADDRINFO=1')
defineIfFuncExists(env, c, 'getnameinfo', 'HAS_GETNAMEINFO=1')
defineIfFuncExists(env, c, 'gethostbyname_r', 'HAS_GETHOSTBYNAME_R=1')
defineIfFuncExists(env, c, 'gethostbyaddr_r', 'HAS_GETHOSTBYADDR_R=1')
defineIfFuncExists(env, c, 'inet_pton', 'HAS_INET_PTON=1')
defineIfFuncExists(env, c, 'inet_ntop', 'HAS_INET_NTOP=1')

DefaultLibraryConfig(env, c)
