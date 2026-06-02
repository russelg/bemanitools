{
  description = "Development environment for bemanitools";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        
        # Python with mdformat
        pythonEnv = pkgs.python3.withPackages (ps: with ps; [
          mdformat
        ]);

        # Cross-compilation toolchains
        mingwW64 = pkgs.pkgsCross.mingwW64.stdenv.cc;
        mingw32 = pkgs.pkgsCross.mingw32.stdenv.cc;

        # MCF threading runtime required by GCC 15+ (--enable-threads=mcf)
        mcfgthreads32 = pkgs.pkgsCross.mingw32.windows.mcfgthreads;
        mcfgthreads64 = pkgs.pkgsCross.mingwW64.windows.mcfgthreads;

      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Build tools
            cmake
            gnumake
            zip
            git
            clang-tools # provides clang-format

            # Compilers
            mingwW64
            mingw32
            pkgs.pkgsCross.mingwW64.windows.pthreads # Common dependency for bemanitools
            mcfgthreads32
            mcfgthreads64

            # Python environment for mdformat
            pythonEnv

            direnv
          ];

          # Set up environment variables to match Docker/Makefile expectations
          shellHook = ''
            echo "--- Bemanitools Dev Environment ---"
            echo "Toolchains available: i686-w64-mingw32- and x86_64-w64-mingw32-"

            # Ensure the toolchain prefixes match what the GNUmakefile expects
            export CROSS_COMPILE_32=i686-w64-mingw32-
            export CROSS_COMPILE_64=x86_64-w64-mingw32-

            # GCC 15+ uses MCF threads; expose libmcfgthread to both cross-linkers
            export NIX_LDFLAGS_i686_w64_mingw32="-L${mcfgthreads32}/lib -lmcfgthread"
            export NIX_LDFLAGS_x86_64_w64_mingw32="-L${mcfgthreads64}/lib -lmcfgthread"
          '';
        };
      });
}
