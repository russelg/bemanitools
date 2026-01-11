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
          '';
        };
      });
}
