namespace rpkg.Utils
{
    static class HashUtils
    {
        public static bool IsLegalHashName(string hashName)
        {
            hashName.ToUpper();

            if (hashName.Length != 16)
            {
                return false;
            }

            string legalCharacters = "0123456789ABCDEF";

            foreach (char a in hashName)
            {
                bool charIsLegal = false;

                foreach (char b in legalCharacters)
                {
                    if (a == b)
                    {
                        charIsLegal = true;
                    }
                }

                if (!charIsLegal)
                {
                    return false;
                }
            }

            return true;
        }

        public static bool IsLegalHashFlag(string hashFlag)
        {
            hashFlag.ToUpper();

            if (hashFlag.Length != 2)
            {
                return false;
            }

            string legalCharacters = "0123456789ABCDEF";

            foreach (char a in hashFlag)
            {
                bool charIsLegal = false;

                foreach (char b in legalCharacters)
                {
                    if (a == b)
                    {
                        charIsLegal = true;
                    }
                }

                if (!charIsLegal)
                {
                    return false;
                }
            }

            return true;
        }
    }
}
