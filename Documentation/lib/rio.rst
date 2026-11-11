=====
rio.h
=====

``rio`` provides wrappers for I/O operations that retry when interrupted by a
signal.

Functions
---------

``rread(fd, buf, count)``
	Read up to ``count`` bytes from ``fd`` into ``buf``.

``rwrite(fd, buf, count)``
	Write up to ``count`` bytes from ``buf`` to ``fd``.

``rftruncate(fd, length)``
	Truncate the file referenced by ``fd`` to ``length`` bytes.

Interruption handling
---------------------

All functions retry the underlying system call when it fails with ``EINTR``.

Other errors are returned directly.

Return value
------------

``rread()`` and ``rwrite()`` return the number of bytes transferred, or -1 on
failure.

``rftruncate()`` returns zero on success, or -1 on failure.

On failure, ``errno`` is set by the underlying system call.
