﻿using System;

using LuaInterface;

namespace BizHawk.Client.Common
{
	public abstract class LuaLibraryBase
	{
		public abstract string Name { get; }
		public abstract string[] Functions { get; }

		public virtual void LuaRegister(Lua lua, ILuaDocumentation docs = null)
		{
			lua.NewTable(Name);
			foreach (var methodName in Functions)
			{
				var func = Name + "." + methodName;
				var method = GetType().GetMethod(Name + "_" + methodName);
				lua.RegisterFunction(func, this, method);

				if (docs != null)
				{
					docs.Add(Name, methodName, method);
				}
			}
		}

		protected static int LuaInt(object luaArg)
		{
			return (int)(double)luaArg;
		}

		protected static uint LuaUInt(object luaArg)
		{
			return (uint)(double)luaArg;
		}

		protected static long LuaLong(object luaArg)
		{
			return (long)(double)luaArg;
		}

		protected static ulong LuaULong(object luaArg)
		{
			return (ulong)(double)luaArg;
		}

		/// <summary>
		/// LuaInterface requires the exact match of parameter count, except optional parameters. 
		/// So, if you want to support variable arguments, declare them as optional and pass
		/// them to this method.
		/// </summary>
		protected static object[] LuaVarArgs(params object[] luaArgs)
		{
			int n = luaArgs.Length;
			int trim = 0;
			for (int i = n - 1; i >= 0; --i)
			{
				if (luaArgs[i] == null)
				{
					++trim;
				}
			}

			var lua_result = new object[n - trim];
			Array.Copy(luaArgs, lua_result, n - trim);
			return lua_result;
		}
	}
}
