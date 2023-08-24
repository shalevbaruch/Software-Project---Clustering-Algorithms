from setuptools import Extension, setup


module = Extension("SymNMF", sources=["symnmfmodule.c", "symnmf.c"])
setup(name="SymNMF", 
      version="1.0",
      description="Implementation on symnmf algoritm",
      ext_modules=[module])

