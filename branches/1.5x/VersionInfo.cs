static class VersionInfo
{
	public const string MAINVERSION = "1.5.3";
	public const string RELEASEDATE = "November 28, 2013";
	public static bool INTERIM = false;

	public static string GetEmuVersion()
	{
		return INTERIM ? "SVN " + SubWCRev.SVN_REV : ("Version " + MAINVERSION);
	}
}
