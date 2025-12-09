import pysnmp.hlapi
import inspect

# Print all available functions/classes in pysnmp.hlapi
print([name for name, obj in inspect.getmembers(pysnmp.hlapi) if inspect.isfunction(obj) or inspect.isclass(obj)])
