// overlong UTF-8 sequence
echo htmlspecialchars("A\xC0\xAF&",     ENT_QUOTES, 'UTF-8');
// invalid Shift_JIS sequence
echo htmlspecialchars("B\x80&",         ENT_QUOTES, 'Shift_JIS');
echo htmlspecialchars("C\x81\x7f&",     ENT_QUOTES, 'Shift_JIS');
// invalid EUC-JP sequence
echo htmlspecialchars("D\x80&",         ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("E\xA1\xFF&",     ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("F\x8E\xFF&",     ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("G\x8F\xA1\xFF&", ENT_QUOTES, 'EUC-JP');